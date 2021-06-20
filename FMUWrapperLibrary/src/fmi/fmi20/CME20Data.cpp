#include <stdarg.h>
#include <string.h>

#include "fmi2.h"
#include "XmlParserCApi.h"
#include "CME20Data.h"
#include "Cconstants.h"

namespace fmuw::fmi20
{

  void fmuLogger(void *componentEnvironment, fmi2String instanceName, fmi2Status status, fmi2String category, fmi2String message, ...)
  {

  }


//---------------------------------------------------------------------------------------------------------------------------------------------
  CME20Data::CME20Data(FMU *fmu, const std::string & path)
    : _Model_po(fmu)
    , _ModulePath_str(path)
    , _Callbacks_po(new fmi2CallbackFunctions({fmuLogger, calloc, free, nullptr, fmu}))
  {
    _ModelID_str = getAttributeValue((Element *)(_Model_po->modelDescription), att_guid);
    _InstanceName_str = getAttributeValue((Element *)getModelExchange(_Model_po->modelDescription), att_modelIdentifier);
  }

  CME20Data::~CME20Data()
  {
    delete _Callbacks_po;
  }

  void CME20Data::freeData()
  {
    if (_IsSimulationEnd == false) {
      if(!_EventInfo_o.terminateSimulation) {
        _Model_po->terminate(_Component_o);
      }
      _Model_po->freeInstance(_Component_o);
      delete [] x;
      delete [] z;
      delete [] xdot;
      delete [] prez;
      x = nullptr;
      z = nullptr;
      xdot = nullptr;
      prez = nullptr;
      _IsSimulationEnd = true;
    }
  }

  void CME20Data::createComponent()
  {
    std::string resourceLocation = "file:///" + _ModulePath_str + "/" + RESOURCES_DIR;
    _Component_o = _Model_po->instantiate(_InstanceName_str.c_str(), fmi2ModelExchange, _ModelID_str.c_str(), resourceLocation.c_str(), _Callbacks_po, fmi2False, fmi2False);
    if(!_Component_o) {
      throw std::logic_error("could not instantiate model");
    }
  }

  void CME20Data::Initialize_v(fmi2Real tStart, fmi2Real tEnd)
  {
    _StartTime_d = tStart;
    _EndTime_d = tEnd;
    ValueStatus vs;
    nx = getDerivativesSize(getModelStructure(_Model_po->modelDescription)); // number of continuous states is number of derivatives
                                                    // declared in model structure
    nz = getAttributeInt((Element *)_Model_po->modelDescription, att_numberOfEventIndicators, &vs); // number of event indicators
    x    = (double *) calloc(nx, sizeof(double));
    xdot = (double *) calloc(nx, sizeof(double));
    if (nz>0) {
        z    =  (double *) calloc(nz, sizeof(double));
        prez =  (double *) calloc(nz, sizeof(double));
    }
    if ((!x || !xdot) || (nz>0 && (!z || !prez))) {
      throw std::out_of_range("Out of memory.");
    }
    fmi2Status fmi2Flag = _Model_po->setupExperiment(_Component_o, fmi2False, 0, tStart, fmi2True, tEnd);
    if (fmi2Flag > fmi2Warning) {
       throw std::logic_error("Could not initialize model. Failed FMI setup experiment.");
    }
    // initialize
    fmi2Flag = _Model_po->enterInitializationMode(_Component_o);
    if (fmi2Flag > fmi2Warning) {
      throw std::logic_error("could not initialize model; failed FMI enter initialization mode");
    }
    fmi2Flag = _Model_po->exitInitializationMode(_Component_o);
    if (fmi2Flag > fmi2Warning) {
      throw std::logic_error("could not initialize model; failed FMI exit initialization mode");
    }
    // event iteration
    _EventInfo_o.newDiscreteStatesNeeded = fmi2True;
    _EventInfo_o.terminateSimulation = fmi2False;
    while (_EventInfo_o.newDiscreteStatesNeeded && !_EventInfo_o.terminateSimulation) {
      // update discrete states
      fmi2Flag = _Model_po->newDiscreteStates(_Component_o, &_EventInfo_o);
      if (fmi2Flag > fmi2Warning) {
        throw std::logic_error("could not set a new discrete state");
      }
    }
    if (!_EventInfo_o.terminateSimulation) { // Работаем до остановки модели
      // enter Continuous-Time Mode
      _Model_po->enterContinuousTimeMode(_Component_o);
    }

    _CurrentTime_d = tStart;
    FormationVarTypes_v();
    FillVariables_v();
    _IsSimulationEnd = false;
  }

  void CME20Data::setBoolData(const std::string &name, const int *value, size_t qty)
  {
    ScalarVariable * sv = getVariable(_Model_po->modelDescription, name.c_str());
    fmi2ValueReference vr = getValueReference(sv);
    _Model_po->setBoolean(_Component_o, &vr, qty, value);
  }

  void CME20Data::setIntData(const std::string &name, const int *value, size_t qty)
  {
    ScalarVariable * sv = getVariable(_Model_po->modelDescription, name.c_str());
    fmi2ValueReference vr = getValueReference(sv);
    _Model_po->setInteger(_Component_o, &vr, qty, value);
  }

  void CME20Data::setRealData(const std::string &name, const double *value, size_t qty)
  {
    ScalarVariable * sv = getVariable(_Model_po->modelDescription, name.c_str());
    fmi2ValueReference vr = getValueReference(sv);
    _Model_po->setReal(_Component_o, &vr, qty, value);
  }

  void CME20Data::setStrData(const std::string &name, const char *value[], size_t qty)
  {
    ScalarVariable * sv = getVariable(_Model_po->modelDescription, name.c_str());
    fmi2ValueReference vr = getValueReference(sv);
    _Model_po->setString(_Component_o, &vr, qty, value);
  }

  void CME20Data::step(double stepSize)
  {
    if (_IsSimulationEnd == true) {
      return;
    }
    if ((_CurrentTime_d == _EndTime_d) || (!x || !xdot || !z || !prez) || _EventInfo_o.terminateSimulation) {
      freeData();
      return;
    }
    // get current state and derivatives
    fmi2Status fmi2Flag = _Model_po->getContinuousStates(_Component_o, x, nx);
    if (fmi2Flag > fmi2Warning) {
      throw std::logic_error("could not retrieve states");
    }
    fmi2Flag = _Model_po->getDerivatives(_Component_o, xdot, nx);
    if (fmi2Flag > fmi2Warning) {
      throw std::logic_error("could not retrieve derivatives");
    }
    double prevTime = _CurrentTime_d;
    _CurrentTime_d = std::min(_CurrentTime_d + stepSize, _EndTime_d);
    fmi2Boolean timeEvent = (_EventInfo_o.nextEventTimeDefined && _EventInfo_o.nextEventTime < _CurrentTime_d);
    if (timeEvent) {
      _CurrentTime_d = _EventInfo_o.nextEventTime;
    }
    double dt = _CurrentTime_d - prevTime;
    fmi2Flag = _Model_po->setTime(_Component_o, _CurrentTime_d);
    if (fmi2Flag > fmi2Warning) {
      throw std::logic_error("could not set time");
    }
    // perform one step
    for (int i = 0; i < nx; ++i) {
      x[i] += dt * xdot[i]; // forward Euler method
    }
    fmi2Flag = _Model_po->setContinuousStates(_Component_o, x, nx);
    if (fmi2Flag > fmi2Warning) {
      throw std::logic_error("could not set states");
    }
    // check for state event
    for (int i = 0; i < nz; ++i) {
      prez[i] = z[i];
    }
    fmi2Flag = _Model_po->getEventIndicators(_Component_o, z, nz);
    if (fmi2Flag > fmi2Warning) {
      throw std::logic_error("could not retrieve event indicators");
    }
    fmi2Boolean stateEvent = FALSE;
    for (int i=0; i < nz; ++i) {
      stateEvent = stateEvent || (prez[i] * z[i] < 0);
    }
    // check for step event, e.g. dynamic state selection
    fmi2Boolean stepEvent, terminateSimulation;
    fmi2Flag = _Model_po->completedIntegratorStep(_Component_o, fmi2True, &stepEvent, &terminateSimulation);
    if (fmi2Flag > fmi2Warning) {
      throw std::logic_error("could not complete intgrator step");
    }
    if (terminateSimulation) {
      freeData(); // success: model requested termination at
      return; // success
    }
    if (timeEvent || stateEvent || stepEvent) {
      _Model_po->enterEventMode(_Component_o);
      // event iteration in one step, ignoring intermediate results
      _EventInfo_o.newDiscreteStatesNeeded = fmi2True;
      _EventInfo_o.terminateSimulation = fmi2False;
      while (_EventInfo_o.newDiscreteStatesNeeded && !_EventInfo_o.terminateSimulation) {
        // update discrete states
        fmi2Flag = _Model_po->newDiscreteStates(_Component_o, &_EventInfo_o);
        if (fmi2Flag > fmi2Warning) {
          throw std::logic_error("could not set a new discrete state");
        }
      }
      // enter Continuous-Time Mode
      _Model_po->enterContinuousTimeMode(_Component_o);
    }
    FillVariables_v();
  }

  const std::map<std::string, int> &CME20Data::outVarNames() const
  {
    return _OutVarNames_map;
  }

  const std::map<std::string, int> &CME20Data::inVarNames() const
  {
    return _InVarNames_map;
  }

  bool CME20Data::boolVar(const std::string &name) const
  {
    if (_BooleansVar_map.find(name) == _BooleansVar_map.end()) {
      throw std::logic_error("No value found");
    }
    return _BooleansVar_map.at(name);
  }

  int CME20Data::intVar(const std::string &name) const
  {
    if (_IntegersVar_map.find(name) == _IntegersVar_map.end()) {
      throw std::logic_error("No value found");
    }
    return _IntegersVar_map.at(name);
  }

  double CME20Data::doubleVar(const std::string &name) const
  {
    if (_DoublesVar_map.find(name) == _DoublesVar_map.end()) {
      throw std::logic_error("No value found");
    }
    return _DoublesVar_map.at(name);
  }

  std::string CME20Data::stringVar(const std::string &name) const
  {
    if (_StringsVar_map.find(name) == _StringsVar_map.end()) {
      throw std::logic_error("No value found");
    }
    return _StringsVar_map.at(name);
  }

  void CME20Data::FormationVarTypes_v()
  {
    int count = getScalarVariableSize(_Model_po->modelDescription);
    for (int i = 0; i < count; ++i) {
      ScalarVariable *sv = getScalarVariable(_Model_po->modelDescription, i);
      const char *name = getAttributeValue((Element *)sv, att_name);
      Elm elementType = getElementType(getTypeSpec(sv));
      printf("NAME >> %s, TYPE >> %d\n", name, elementType);
      int type = -1;
      switch(elementType) {
      case elm_Real: type = FTReal_en; break;
      case elm_Integer: case elm_Enumeration: type = FTInteger_en; break;
      case elm_Boolean: type = FTBoolean_en; break;
      case elm_String: type = FTString_en; break;
      default: break;
      }
      Enu causality = getCausality(sv);
      Enu variability = getVariability(sv);
      if (variability != enu_parameter) {
        if (causality == enu_input) {
          _InVarNames_map[name] = type;
        } else if (causality == enu_local || causality == enu_output) {
          _OutVarNames_map[name] = type;
        }
      }
    }
  }

  void CME20Data::FillVariables_v()
  {
    int count = getScalarVariableSize(_Model_po->modelDescription);
    for (int i = 0; i < count; ++i) {
      ScalarVariable *sv = getScalarVariable(_Model_po->modelDescription, i);
      fmi2ValueReference vr = getValueReference(sv);
      const char *name = getAttributeValue((Element *)sv, att_name);
      Elm elementType = getElementType(getTypeSpec(sv));
      switch (elementType) {
      case elm_Real: {
        fmi2Real variable;
        _Model_po->getReal(_Component_o, &vr, 1, &variable);
        _DoublesVar_map[name] = variable;
      } break;
      case elm_Integer:
      case elm_Enumeration: {
        fmi2Integer variable;
        _Model_po->getInteger(_Component_o, &vr, 1, &variable);
        _IntegersVar_map[name] = variable;
      } break;
      case elm_Boolean: {
        fmi2Boolean variable;
        _Model_po->getBoolean(_Component_o, &vr, 1, &variable);
      } break;
      case elm_String: {
        fmi2String variable;
        _Model_po->getString(_Component_o, &vr, 1, &variable);
      } break;
      default: {
        throw std::logic_error("No value for type = " + std::to_string(elementType));
      }
      }
    }
  }

} // namespace fmuw::fmi20

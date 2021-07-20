#include <stdarg.h>
#include <string.h>
#include <stdexcept>

#include "fmi2.h"
#include "XmlParserCApi.h"
#include "CCS20Data.h"
#include "Cconstants.h"

namespace fmuw
{
  namespace fmi20
  {

    void fmu20csLogger(void* componentEnvironment, fmi2String instanceName, fmi2Status status, fmi2String category, fmi2String message, ...)
    {

    }


    //---------------------------------------------------------------------------------------------------------------------------------------------
    CCS20Data::CCS20Data(FMU* fmu, const std::string& path)
      : _Model_po(fmu)
      , _ModulePath_str(path)
      , _Callbacks_po(new fmi2CallbackFunctions({ fmu20csLogger, calloc, free, nullptr, fmu }))
    {
      _ModelID_str = getAttributeValue((Element*)(_Model_po->modelDescription), att_guid);
      _InstanceName_str = getAttributeValue((Element*)getCoSimulation(_Model_po->modelDescription), att_modelIdentifier);
    }

    CCS20Data::~CCS20Data()
    {
      delete _Callbacks_po;
    }

    void CCS20Data::freeData()
    {
      if (_IsSimulationEnd == false) {
        if (!_EventInfo_o.terminateSimulation) {
          _Model_po->terminate(_Component_o);
        }
        _Model_po->freeInstance(_Component_o);
        _IsSimulationEnd = true;
      }
    }

    void CCS20Data::createComponent()
    {
      std::string resourceLocation = "file:///" + _ModulePath_str + "/" + RESOURCES_DIR;
      _Component_o = _Model_po->instantiate(_InstanceName_str.c_str(), fmi2CoSimulation, _ModelID_str.c_str(), resourceLocation.c_str(), _Callbacks_po, fmi2False, fmi2False);
      if (!_Component_o) {
        throw std::logic_error("could not instantiate model");
      }
    }

    void CCS20Data::Initialize_v(fmi2Real tStart, fmi2Real tEnd)
    {
      _StartTime_d = tStart;
      _EndTime_d = tEnd;
      fmi2Boolean toleranceDefined = fmi2False;  // true if model description define tolerance
      fmi2Real tolerance = 0.0;
      Element* defaultExp = getDefaultExperiment(_Model_po->modelDescription);
      ValueStatus vs = valueMissing;
      if (defaultExp) {
        tolerance = getAttributeDouble(defaultExp, att_tolerance, &vs);
      }
      if (vs == valueDefined) {
        toleranceDefined = fmi2True;
      }
      fmi2Status fmi2Flag = _Model_po->setupExperiment(_Component_o, toleranceDefined, tolerance, tStart, fmi2True, tEnd);
      if (fmi2Flag > fmi2Warning) {
        throw std::logic_error("could not initialize model; failed FMI setup experiment");
      }

      fmi2Flag = _Model_po->enterInitializationMode(_Component_o);
      if (fmi2Flag > fmi2Warning) {
        throw std::logic_error("could not initialize model; failed FMI enter initialization mode");
      }
      fmi2Flag = _Model_po->exitInitializationMode(_Component_o);
      if (fmi2Flag > fmi2Warning) {
        throw std::logic_error("could not initialize model; failed FMI exit initialization mode");
      }

      _CurrentTime_d = tStart;
      FormationVarTypes_v();
      FillVariables_v();
      _IsSimulationEnd = false;
    }

    void CCS20Data::setBoolData(const std::string& name, const int* value, size_t qty)
    {
      ScalarVariable* sv = getVariable(_Model_po->modelDescription, name.c_str());
      fmi2ValueReference vr = getValueReference(sv);
      fmi2Status status = _Model_po->setBoolean(_Component_o, &vr, qty, value);
      if (status != fmi2OK) {
        /// @todo Сделать обработку ошибки
      }
    }

    void CCS20Data::setIntData(const std::string& name, const int* value, size_t qty)
    {
      ScalarVariable* sv = getVariable(_Model_po->modelDescription, name.c_str());
      fmi2ValueReference vr = getValueReference(sv);
      fmi2Status status = _Model_po->setInteger(_Component_o, &vr, qty, value);
      if (status != fmi2OK) {
        /// @todo Сделать обработку ошибки
      }
    }

    void CCS20Data::setRealData(const std::string& name, const double* value, size_t qty)
    {
      ScalarVariable* sv = getVariable(_Model_po->modelDescription, name.c_str());
      fmi2ValueReference vr = getValueReference(sv);
      fmi2Status status = _Model_po->setReal(_Component_o, &vr, qty, value);
      if (status != fmi2OK) {
        /// @todo Сделать обработку ошибки
      }
    }

    void CCS20Data::setStrData(const std::string& name, const char* value[], size_t qty)
    {
      ScalarVariable* sv = getVariable(_Model_po->modelDescription, name.c_str());
      fmi2ValueReference vr = getValueReference(sv);
      fmi2Status status = _Model_po->setString(_Component_o, &vr, qty, value);
      if (status != fmi2OK) {
        /// @todo Сделать обработку ошибки
      }
    }

    void CCS20Data::step(double stepSize)
    {
      if (_IsSimulationEnd == true) {
        return;
      }
      if (_CurrentTime_d >= _EndTime_d) {
        freeData();
        return;
      }
      double dt = stepSize;
      if (stepSize > _EndTime_d - _CurrentTime_d) {
        dt = _EndTime_d - _CurrentTime_d;
      }
      fmi2Status fmi2Flag = _Model_po->doStep(_Component_o, _CurrentTime_d, dt, fmi2True);
      if (fmi2Flag == fmi2Discard) {
        fmi2Boolean b;
        // check if model requests to end simulation
        if (fmi2OK != _Model_po->getBooleanStatus(_Component_o, fmi2Terminated, &b)) {
          throw std::logic_error("could not complete simulation of the model. getBooleanStatus return other than fmi2OK");
        }
        if (b == fmi2True) {
          throw std::logic_error("the model requested to end the simulation");
        }
        throw std::logic_error("could not complete simulation of the model");
      }
      if (fmi2Flag != fmi2OK) {
        std::logic_error("could not complete simulation of the model");
      }
      _CurrentTime_d += dt;
      FillVariables_v();
    }

    const std::map<std::string, int>& CCS20Data::outVarNames() const
    {
      return _OutVarNames_map;
    }

    const std::map<std::string, int>& CCS20Data::inVarNames() const
    {
      return _InVarNames_map;
    }

    bool CCS20Data::boolVar(const std::string& name) const
    {
      if (_BooleansVar_map.find(name) == _BooleansVar_map.end()) {
        throw std::logic_error("No value found");
      }
      return _BooleansVar_map.at(name);
    }

    int CCS20Data::intVar(const std::string& name) const
    {
      if (_IntegersVar_map.find(name) == _IntegersVar_map.end()) {
        throw std::logic_error("No value found");
      }
      return _IntegersVar_map.at(name);
    }

    double CCS20Data::doubleVar(const std::string& name) const
    {
      if (_DoublesVar_map.find(name) == _DoublesVar_map.end()) {
        throw std::logic_error("No value found");
      }
      return _DoublesVar_map.at(name);
    }

    std::string CCS20Data::stringVar(const std::string& name) const
    {
      if (_StringsVar_map.find(name) == _StringsVar_map.end()) {
        throw std::logic_error("No value found");
      }
      return _StringsVar_map.at(name);
    }

    void CCS20Data::FormationVarTypes_v()
    {
      int count = getScalarVariableSize(_Model_po->modelDescription);
      for (int i = 0; i < count; ++i) {
        ScalarVariable* sv = getScalarVariable(_Model_po->modelDescription, i);
        const char* name = getAttributeValue((Element*)sv, att_name);
        Elm elementType = getElementType(getTypeSpec(sv));
        printf("NAME >> %s, TYPE >> %d\n", name, elementType);
        int type = -1;
        switch (elementType) {
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
          }
          else if (causality == enu_local || causality == enu_output) {
            _OutVarNames_map[name] = type;
          }
        }
      }
    }

    void CCS20Data::FillVariables_v()
    {
      int count = getScalarVariableSize(_Model_po->modelDescription);
      for (int i = 0; i < count; ++i) {
        fmi2Status status = fmi2OK;
        ScalarVariable* sv = getScalarVariable(_Model_po->modelDescription, i);
        fmi2ValueReference vr = getValueReference(sv);
        const char* name = getAttributeValue((Element*)sv, att_name);
        Elm elementType = getElementType(getTypeSpec(sv));
        switch (elementType) {
        case elm_Real: {
          fmi2Real variable;
          status = _Model_po->getReal(_Component_o, &vr, 1, &variable);
          _DoublesVar_map[name] = variable;
        } break;
        case elm_Integer:
        case elm_Enumeration: {
          fmi2Integer variable;
          status = _Model_po->getInteger(_Component_o, &vr, 1, &variable);
          _IntegersVar_map[name] = variable;
        } break;
        case elm_Boolean: {
          fmi2Boolean variable;
          status = _Model_po->getBoolean(_Component_o, &vr, 1, &variable);
          _BooleansVar_map[name] = variable;
        } break;
        case elm_String: {
          fmi2String variable;
          status = _Model_po->getString(_Component_o, &vr, 1, &variable);
          _StringsVar_map[name] = variable;
        } break;
        default: {
          throw std::logic_error("No value for type = " + std::to_string(elementType));
        }
        }
        if (status != fmi2OK) {
          /// @todo Действия по выбору ошибки
        }
      }
    }
  } // namespace fmi20
} // namespace fmuw

#include <stdexcept>

#include "CMEData.h"
#include "Cconstants.h"
#include "fmi_me.h"

namespace fmuw
{
  namespace fmi10
  {

    CMEData::CMEData(const char* guid, FMU* fmu)
      : _ModelID_str(guid)
      , _Model_po(fmu)
    {

    }

    CMEData::~CMEData()
    {

    }

    void CMEData::freeData() {
      if (_IsSimulationEnd == false) {
        if (!_EventInfo_o.terminateSimulation) {
          _Model_po->terminate(_Component_o);
        }
        _Model_po->freeModelInstance(_Component_o);
        delete[] x;
        delete[] z;
        delete[] xdot;
        delete[] prez;
        x = nullptr;
        z = nullptr;
        xdot = nullptr;
        prez = nullptr;
        _IsSimulationEnd = true;
      }
    }

    void CMEData::createComponent() {
      _Callbacks_o.logger = fmuLogger;
      _Callbacks_o.allocateMemory = calloc;
      _Callbacks_o.freeMemory = free;
      _Component_o = _Model_po->instantiateModel(getModelIdentifier(_Model_po->modelDescription)
        , _ModelID_str.c_str(), _Callbacks_o, fmiFalse);
      if (!_Component_o) {
        throw std::logic_error("could not instantiate model");
      }
    }

    void CMEData::Initialize_v(fmiReal tStart, fmiReal tEnd)
    {
      _StartTime_d = tStart;
      _EndTime_d = tEnd;
      fmiStatus fmiFlag = _Model_po->setTime(_Component_o, _StartTime_d);
      if (fmiFlag > fmiWarning) {
        throw std::logic_error("Could not set time");
      }
      fmiFlag = _Model_po->initialize(_Component_o, fmiFalse, tStart, &_EventInfo_o);
      if (fmiFlag > fmiWarning) {
        throw std::logic_error("Could not initialize model");
      }
      if (_EventInfo_o.terminateSimulation) {
        // model requested termination at init;
        _EndTime_d = tStart;
      }
      nx = getNumberOfStates(_Model_po->modelDescription);
      nz = getNumberOfEventIndicators(_Model_po->modelDescription);

      x = new double[nx];
      xdot = new double[nx];

      if (nz > 0) {
        z = new double[nz];
        prez = new double[nz];
      }

      FormationVarTypes_v();
      FillVariables_v();
      _IsSimulationEnd = false;
    }

    void CMEData::setVarData(const std::string& name, const char* value, size_t qty)
    {
      ScalarVariable* sv = getVariableByName(_Model_po->modelDescription, name.c_str());
      fmiValueReference vr = getValueReference(sv);
      _Model_po->setBoolean(_Component_o, &vr, qty, value);
    }

    void CMEData::setVarData(const std::string& name, const int* value, size_t qty)
    {
      ScalarVariable* sv = getVariableByName(_Model_po->modelDescription, name.c_str());
      fmiValueReference vr = getValueReference(sv);
      _Model_po->setInteger(_Component_o, &vr, qty, value);
    }

    void CMEData::setVarData(const std::string& name, const double* value, size_t qty)
    {
      ScalarVariable* sv = getVariableByName(_Model_po->modelDescription, name.c_str());
      fmiValueReference vr = getValueReference(sv);
      _Model_po->setReal(_Component_o, &vr, qty, value);
    }

    void CMEData::setVarData(const std::string& name, const char* value[], size_t qty)
    {
      ScalarVariable* sv = getVariableByName(_Model_po->modelDescription, name.c_str());
      fmiValueReference vr = getValueReference(sv);
      _Model_po->setString(_Component_o, &vr, qty, value);
    }

    void CMEData::step(double stepSize)
    {
      if (_IsSimulationEnd == true) {
        return;
      }
      if ((_CurrentTime_d == _EndTime_d) || (!x || !xdot || !z || !prez)) {
        freeData();
        return;
      }
      // get current state and derivatives
      fmiStatus fmiFlag = _Model_po->getContinuousStates(_Component_o, x, nx);
      if (fmiFlag > fmiWarning) {
        throw std::logic_error("could not retrieve states");
      }
      fmiFlag = _Model_po->getDerivatives(_Component_o, xdot, nx);
      if (fmiFlag > fmiWarning) {
        throw std::logic_error("could not retrieve derivatives");
      }
      double prevTime = _CurrentTime_d;
      _CurrentTime_d = min(_CurrentTime_d + stepSize, _EndTime_d);
      bool timeEvent = _EventInfo_o.upcomingTimeEvent && _EventInfo_o.nextEventTime < _CurrentTime_d;
      if (timeEvent) {
        _CurrentTime_d = _EventInfo_o.nextEventTime;
      }
      double dt = _CurrentTime_d - prevTime;
      fmiFlag = _Model_po->setTime(_Component_o, _CurrentTime_d);
      if (fmiFlag > fmiWarning) {
        throw std::logic_error("could not set time");
      }
      // perform one step
      for (int i = 0; i < nx; ++i) { // forward Euler method
        x[i] += dt * xdot[i];
      }
      fmiFlag = _Model_po->setContinuousStates(_Component_o, x, nx);
      if (fmiFlag > fmiWarning) {
        throw std::logic_error("could not set states");
      }
      // Check for step event, e.g. dynamic state selection
      fmiBoolean stepEvent;
      fmiFlag = _Model_po->completedIntegratorStep(_Component_o, &stepEvent);
      if (fmiFlag > fmiWarning) {
        throw std::logic_error("could not complete integrator step");
      }
      // Check for state event
      for (int i = 0; i < nz; ++i) {
        prez[i] = z[i];
      }
      fmiFlag = _Model_po->getEventIndicators(_Component_o, z, nz);
      if (fmiFlag > fmiWarning) {
        throw std::logic_error("could not retrieve event indicators");
      }
      bool stateEvent = FALSE;
      for (int i = 0; i < nz; ++i) {
        stateEvent = stateEvent || (prez[i] * z[i] < 0);
      }
      // handle events
      if (timeEvent || stateEvent || stepEvent) {
        // event iteration in one step, ignoring intermediate results
        fmiFlag = _Model_po->eventUpdate(_Component_o, fmiFalse, &_EventInfo_o);
        if (fmiFlag > fmiWarning) {
          throw std::logic_error("could not perform event update");
        }
        // terminate simulation, if requested by the model
        if (_EventInfo_o.terminateSimulation) {
          freeData(); // success: model requested termination at
        }
      }
      FillVariables_v();
    }

    bool CMEData::boolVar(const std::string& name) const
    {
      if (_BooleansVar_map.find(name) == _BooleansVar_map.end()) {
        throw std::logic_error("No value found");
      }
      return _BooleansVar_map.at(name);
    }

    int CMEData::intVar(const std::string& name) const
    {
      if (_IntegersVar_map.find(name) == _IntegersVar_map.end()) {
        throw std::logic_error("No value found");
      }
      return _IntegersVar_map.at(name);
    }

    double CMEData::doubleVar(const std::string& name) const
    {
      if (_DoublesVar_map.find(name) == _DoublesVar_map.end()) {
        throw std::logic_error("No value found");
      }
      return _DoublesVar_map.at(name);
    }

    std::string CMEData::stringVar(const std::string& name) const
    {
      if (_StringsVar_map.find(name) == _StringsVar_map.end()) {
        throw std::logic_error("No value found");
      }
      return _StringsVar_map.at(name);
    }

    void CMEData::FormationVarTypes_v()
    {
      ScalarVariable** vars = _Model_po->modelDescription->modelVariables;
      for (int i = 0; vars[i]; ++i) {
        ScalarVariable* sv = vars[i];
        if (getAlias(sv) != fmi10::enu_noAlias) {
          continue;
        }
        std::string name = getName(sv);
        int type = -1;
        switch (sv->typeSpec->type) {
        case elm_Real: type = FTReal_en; break;
        case elm_Integer: case elm_Enumeration: type = FTInteger_en; break;
        case elm_Boolean: type = FTBoolean_en; break;
        case elm_String: type = FTString_en; break;
        default: break;
        }
        Enu Casuality_en = getCausality(sv);
        Enu Alias_en = getAlias(sv);
        Enu Variability_en = getVariability(sv);
        if (Variability_en != enu_parameter) { // Параметры модели нас (пока) не интересуют
          if (Casuality_en == enu_input) {
            _InVarNames_map[name] = type;
          }
          else {
            _OutVarNames_map[name] = type;
          }
        }
      }
    }

    void CMEData::FillVariables_v()
    {
      ScalarVariable** vars = _Model_po->modelDescription->modelVariables;
      for (int i = 0; vars[i]; ++i) {
        ScalarVariable* sv = vars[i];
        if (getAlias(sv) != fmi10::enu_noAlias) {
          continue;
        }
        std::string name = fmi10::getName(sv);
        if (_OutVarNames_map.find(name) != _OutVarNames_map.end()) { // Только для обрабатываемых данных
          fmiValueReference vr = getValueReference(sv);
          switch (sv->typeSpec->type) {
          case elm_Real: {
            fmiReal variable;
            _Model_po->getReal(_Component_o, &vr, 1, &variable);
            _DoublesVar_map[name] = variable;
          } break;
          case elm_Integer:
          case elm_Enumeration: {
            fmiInteger variable;
            _Model_po->getInteger(_Component_o, &vr, 1, &variable);
            _IntegersVar_map[name] = variable;
          } break;
          case elm_Boolean: {
            fmiBoolean variable;
            _Model_po->getBoolean(_Component_o, &vr, 1, &variable);
          } break;
          case elm_String: {
            fmiString variable;
            _Model_po->getString(_Component_o, &vr, 1, &variable);
          } break;
          default: {
            throw std::logic_error("No value for type = " + std::to_string(sv->typeSpec->type));
          }
          }
        }
      }
    }
  } // namespace fmi10
} //namespace fmuw

#include <stdexcept>
#include <stdarg.h>
#include <iostream>

#include "CCSData.h"
#include "Cconstants.h"
#include "fmi_cs.h"

namespace fmuw::fmi10 {

  CCSData::CCSData(const char * guid, FMU * fmu)
  : _ModelID_str(guid)
  , _Model_po(fmu)
  {

  }

  CCSData::~CCSData()
  {

  }

  void CCSData::freeData()
  {
    if (_IsSimulationEnd == false) {
      _Model_po->terminateSlave(_Component_o);
      _Model_po->freeSlaveInstance(_Component_o);
      _SaveStepSize_d = 0.0;
      _IsSimulationEnd = true;
    }
  }

  void CCSData::createComponent()
  {
    _Callbacks_o.logger = fmuLogger;
    _Callbacks_o.allocateMemory = calloc;
    _Callbacks_o.freeMemory = free;
    _Callbacks_o.stepFinished = NULL; // fmiDoStep has to be carried out synchronously
    const char* fmuLocation = ""; // path to the fmu as URL, "file://C:\QTronic\sales"
    const char* mimeType = "application/x-fmu-sharedlibrary"; // denotes tool in case of tool coupling
    fmiReal timeout = 1000;          // wait period in milli seconds, 0 for unlimited wait period"
    fmiBoolean visible = fmiFalse;   // no simulator user interface
    fmiBoolean interactive = fmiFalse; // simulation run without user interaction
    _Component_o = _Model_po->instantiateSlave(getModelIdentifier(_Model_po->modelDescription), _ModelID_str.c_str(), fmuLocation
                                               , mimeType, timeout, visible, interactive, _Callbacks_o, false);
    if (!_Component_o) {
      throw std::logic_error("could not instantiate model");
    }
  }

  void CCSData::Initialize_v(fmiReal tStart, fmiReal tEnd)
  {
    // StopTimeDefined=fmiFalse means: ignore value of tEnd
    _EndTime_d = tEnd;
    _StartTime_d = tStart;
    fmiStatus fmiFlag = _Model_po->initializeSlave(_Component_o, tStart, fmiTrue, tEnd);
    if (fmiFlag > fmiWarning)  {
      throw std::logic_error("could not initialize model");
    }
    _CurrentTime_d = tStart;
    FormationVarTypes_v();
    FillVariables_v();
    _IsSimulationEnd = false;
  }

  void CCSData::setVarData(const std::string &name, const char *value, size_t qty)
  {
    ScalarVariable* sv = getVariableByName(_Model_po->modelDescription, name.c_str());
    fmiValueReference vr = getValueReference(sv);
    fmiStatus status = _Model_po->setBoolean(_Component_o, &vr, qty, value);
    if (status != fmiOK) {
      /// @todo Сделать обработку ошибок
    }
  }

  void CCSData::setVarData(const std::string &name, const int *value, size_t qty)
  {
    ScalarVariable* sv = getVariableByName(_Model_po->modelDescription, name.c_str());
    fmiValueReference vr = getValueReference(sv);
    fmiStatus status = _Model_po->setInteger(_Component_o, &vr, qty, value);
    if (status != fmiOK) {
      /// @todo Сделать обработку ошибок
    }
  }

  void CCSData::setVarData(const std::string &name, const double *value, size_t qty)
  {
    ScalarVariable* sv = getVariableByName(_Model_po->modelDescription, name.c_str());
    fmiValueReference vr = getValueReference(sv);
    fmiStatus status = _Model_po->setReal(_Component_o, &vr, qty, value);
    if (status != fmiOK) {
      /// @todo Сделать обработку ошибок
    }
  }

  void CCSData::setVarData(const std::string &name, const char *value[], size_t qty)
  {
    ScalarVariable* sv = getVariableByName(_Model_po->modelDescription, name.c_str());
    fmiValueReference vr = getValueReference(sv);
    fmiStatus status = _Model_po->setString(_Component_o, &vr, qty, value);
    if (status != fmiOK) {
      /// @todo Сделать обработку ошибок
    }
  }

  void CCSData::step(double stepSize)
  {
    if (_IsSimulationEnd == true) {
      return;
    }
    if (_CurrentTime_d == _EndTime_d) {
      freeData();
      return;
    }
    if (_SaveStepSize_d == 0.0) {
      _SaveStepSize_d = stepSize;
    }
    // check not to pass over end time
    if (stepSize > _EndTime_d - _CurrentTime_d) {
        _SaveStepSize_d = _EndTime_d - _CurrentTime_d;
    }
    fmiStatus fmiFlag = _Model_po->doStep(_Component_o, _CurrentTime_d, _SaveStepSize_d, fmiTrue);
    if (fmiFlag != fmiOK) {
      throw std::logic_error("could not complete simulation of the model");
    }
    _CurrentTime_d += _SaveStepSize_d;
    FillVariables_v();
  }

  bool CCSData::boolVar(const std::string &name) const
  {
    if (_BooleansVar_map.find(name) == _BooleansVar_map.end()) {
      throw std::logic_error("No value found");
    }
    return _BooleansVar_map.at(name);
  }

  int CCSData::intVar(const std::string &name) const
  {
    if (_IntegersVar_map.find(name) == _IntegersVar_map.end()) {
      throw std::logic_error("No value found");
    }
    return _IntegersVar_map.at(name);
  }

  double CCSData::doubleVar(const std::string &name) const
  {
    if (_DoublesVar_map.find(name) == _DoublesVar_map.end()) {
      throw std::logic_error("No value found");
    }
    return _DoublesVar_map.at(name);
  }

  std::string CCSData::stringVar(const std::string &name) const
  {
    if (_StringsVar_map.find(name) == _StringsVar_map.end()) {
      throw std::logic_error("No value found");
    }
    return _StringsVar_map.at(name);
  }

  void CCSData::FormationVarTypes_v()
  {
    ScalarVariable** vars = _Model_po->modelDescription->modelVariables;
    for (int i = 0; vars[i]; ++i) {
      ScalarVariable* sv = vars[i];
      if (getAlias(sv)!=fmi10::enu_noAlias) {
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
        } else {
          _OutVarNames_map[name] = type;
        }
      }
    }
  }

  void CCSData::FillVariables_v()
  {
    ScalarVariable** vars = _Model_po->modelDescription->modelVariables;
    for (int i = 0; vars[i]; ++i) {
      fmiStatus status = fmiOK;
      ScalarVariable* sv = vars[i];
      if (getAlias(sv)!=fmi10::enu_noAlias) {
        continue;
      }
      std::string name = fmi10::getName(sv);
      if (_OutVarNames_map.find(name) != _OutVarNames_map.end()) { // Только для обрабатываемых данных
        fmiValueReference vr = getValueReference(sv);
        switch (sv->typeSpec->type){
        case elm_Real: {
          fmiReal variable;
          status = _Model_po->getReal(_Component_o, &vr, 1, &variable);
          _DoublesVar_map[name] = variable;
        } break;
        case elm_Integer:
        case elm_Enumeration: {
          fmiInteger variable;
          status = _Model_po->getInteger(_Component_o, &vr, 1, &variable);
          _IntegersVar_map[name] = variable;
        } break;
        case elm_Boolean: {
          fmiBoolean variable;
          status = _Model_po->getBoolean(_Component_o, &vr, 1, &variable);
          _BooleansVar_map[name] = variable;
        } break;
        case elm_String: {
          fmiString variable;
          status = _Model_po->getString(_Component_o, &vr, 1, &variable);
          _StringsVar_map[name] = variable;
        } break;
        default: {
          throw std::logic_error("No value for type = " + std::to_string(sv->typeSpec->type));
        }
        }
      }
      if (status != fmiOK) {
        /// @todo Сделать обработку ошибок
      }
    }
  }

} // namespace fmuw::fmi10

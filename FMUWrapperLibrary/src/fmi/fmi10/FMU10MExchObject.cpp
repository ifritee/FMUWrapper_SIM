#include <iostream>
#include <stdexcept>

#ifdef WINDOWS
  #include <windows.h>
#else
  #include <dlfcn.h>
#endif

#include "FMU10MExchObject.h"
#include "Cconstants.h"
#include "xml_parser10.h"
#include "fmi_me.h"

namespace fmuw
{
  void fmuLogger(fmiComponent c, fmiString instanceName, fmiStatus status,
                 fmiString category, fmiString message, ...) {}

  /**
   * @class CModelData
   * @brief Данные модели */
  class CModelData {
  public:
    /** @brief Конструктор */
    explicit CModelData(const char * guid, fmi10::FMU * fmu)
      : _ModelID_str(guid)
      , _Model_po(fmu)
    {}

    virtual ~CModelData() {

    }

    void freeData() {
      if(!_EventInfo_o.terminateSimulation) {
        _Model_po->terminate(_Component_o);
      }
      _Model_po->freeModelInstance(_Component_o);
    }

    void createComponent() {
      _Callbacks_o.logger = fmuLogger;
      _Callbacks_o.allocateMemory = calloc;
      _Callbacks_o.freeMemory = free;
      _Component_o = _Model_po->instantiateModel(fmi10::getModelIdentifier(_Model_po->modelDescription)
                                                 , _ModelID_str.c_str(), _Callbacks_o, fmiFalse);
      if (!_Component_o) {
        throw std::logic_error("could not instantiate model");
      }
    }

    void initialize(fmiReal tStart, fmiReal tEnd)
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
      _VarStates_i = getNumberOfStates(_Model_po->modelDescription);
      _VarEvents_i = getNumberOfEventIndicators(_Model_po->modelDescription);

//      // output solution for time t0
//      outputRow(fmu, c, 0.0, file, '|', fmiTrue);  // output column names
//      outputRow(fmu, c, 0.0, file, '|', fmiFalse); // output values

    }
    /** @brief Возвращает Количество состояний переменных */
    int varStates() const { return _VarStates_i; }
    /** @brief Возвращает Количество состояний событий переменных */
    int varEvents() const { return _VarEvents_i; }

  private:
    fmi10::FMU * _Model_po;
    std::string _ModelID_str; ///< @brief global unique id of the fmu
    fmiComponent _Component_o; ///< @brief Компонент FMU
    fmiReal _StartTime_d = 0.0; ///< @brief Время запуска
    fmiReal _EndTime_d = 0.0; ///< @brief Время останова
    fmiEventInfo _EventInfo_o; ///< @brief Обновление при вызове, инициализации и обновлении прерывания
    fmiCallbackFunctions _Callbacks_o;
    int _VarStates_i = 0; ///< @brief Количество состояний переменных
    int _VarEvents_i = 0; ///< @brief Количество состояний событий переменных
  };
//------------------------------------------------------------

  FMU10MExchObject::FMU10MExchObject(const std::string &path)
    : FMU10ObjectAbstract(path)
    , _ModelUnit_po(new fmi10::FMU)
  {

  }

  FMU10MExchObject::~FMU10MExchObject()
  {
    _ModelData_po->freeData();
#if WINDOWS
    FreeLibrary(_ModelUnit_po->dllHandle);
#else /* WINDOWS */
    dlclose(_ModelUnit_po->dllHandle);
#endif /* WINDOWS */
    delete _ModelData_po;
    delete _ModelUnit_po;
  }

  void FMU10MExchObject::parse(std::string &fileName)
  {
    _ModelUnit_po->modelDescription = fmi10::parse(fileName.c_str());
    if (!_ModelUnit_po->modelDescription) {
      throw std::runtime_error("Model descriptor is crash");
    }
    printModelDescription(_ModelUnit_po->modelDescription);
    _FilenameDLL_str = _PathToFMU_str + "/" + LIBRARY_PATH
        + fmi10::getModelIdentifier(_ModelUnit_po->modelDescription) + LIBRARY_SUFFIX;
  }

  void FMU10MExchObject::loadLibrary()
  {
#if WINDOWS
    HANDLE handle = LoadLibrary(dllPath);
#else /* WINDOWS */
    HANDLE handle = dlopen(_FilenameDLL_str.c_str(), RTLD_LAZY);
#endif /* WINDOWS */
    if (!handle) {
#if WINDOWS
      throw std::runtime_error(std::string("The error was: ") + std::to_string(GetLastError()));
#else
      throw std::runtime_error(std::string("The error was: ") + dlerror());
#endif
    }
    _ModelUnit_po->dllHandle = handle;
    //----- Загрузка методов -----
    _ModelUnit_po->getModelTypesPlatform   = (fmi10::fGetModelTypesPlatform) getFunctionAddress("fmiGetModelTypesPlatform");
    _ModelUnit_po->instantiateModel        = (fmi10::fInstantiateModel)   getFunctionAddress("fmiInstantiateModel");
    _ModelUnit_po->freeModelInstance       = (fmi10::fFreeModelInstance)  getFunctionAddress("fmiFreeModelInstance");
    _ModelUnit_po->setTime                 = (fmi10::fSetTime)            getFunctionAddress("fmiSetTime");
    _ModelUnit_po->setContinuousStates     = (fmi10::fSetContinuousStates)getFunctionAddress("fmiSetContinuousStates");
    _ModelUnit_po->completedIntegratorStep = (fmi10::fCompletedIntegratorStep)getFunctionAddress("fmiCompletedIntegratorStep");
    _ModelUnit_po->initialize              = (fmi10::fInitialize)         getFunctionAddress("fmiInitialize");
    _ModelUnit_po->getDerivatives          = (fmi10::fGetDerivatives)     getFunctionAddress("fmiGetDerivatives");
    _ModelUnit_po->getEventIndicators      = (fmi10::fGetEventIndicators) getFunctionAddress("fmiGetEventIndicators");
    _ModelUnit_po->eventUpdate             = (fmi10::fEventUpdate)        getFunctionAddress("fmiEventUpdate");
    _ModelUnit_po->getContinuousStates     = (fmi10::fGetContinuousStates)getFunctionAddress("fmiGetContinuousStates");
    _ModelUnit_po->getNominalContinuousStates = (fmi10::fGetNominalContinuousStates)getFunctionAddress("fmiGetNominalContinuousStates");
    _ModelUnit_po->getStateValueReferences = (fmi10::fGetStateValueReferences)getFunctionAddress("fmiGetStateValueReferences");
    _ModelUnit_po->terminate               = (fmi10::fTerminate)          getFunctionAddress("fmiTerminate");
    _ModelUnit_po->getVersion              = (fmi10::fGetVersion)         getFunctionAddress("fmiGetVersion");
    _ModelUnit_po->setDebugLogging         = (fmi10::fSetDebugLogging)    getFunctionAddress("fmiSetDebugLogging");
    _ModelUnit_po->setReal                 = (fmi10::fSetReal)            getFunctionAddress("fmiSetReal");
    _ModelUnit_po->setInteger              = (fmi10::fSetInteger)         getFunctionAddress("fmiSetInteger");
    _ModelUnit_po->setBoolean              = (fmi10::fSetBoolean)         getFunctionAddress("fmiSetBoolean");
    _ModelUnit_po->setString               = (fmi10::fSetString)          getFunctionAddress("fmiSetString");
    _ModelUnit_po->getReal                 = (fmi10::fGetReal)            getFunctionAddress("fmiGetReal");
    _ModelUnit_po->getInteger              = (fmi10::fGetInteger)         getFunctionAddress("fmiGetInteger");
    _ModelUnit_po->getBoolean              = (fmi10::fGetBoolean)         getFunctionAddress("fmiGetBoolean");
    _ModelUnit_po->getString               = (fmi10::fGetString)          getFunctionAddress("fmiGetString");
  }

  void FMU10MExchObject::initialize(double endTime)
  {
    if (_ModelData_po != nullptr) {
      delete _ModelData_po;
    }
    _ModelData_po = new CModelData(getString(_ModelUnit_po->modelDescription, fmi10::att_guid), _ModelUnit_po);
    _ModelData_po->createComponent(/*calloc, free*/);
    _ModelData_po->initialize(0.0, endTime);
  }

  void FMU10MExchObject::printModelDescription(fmi10::ModelDescription* md)
  {
    fmi10::Element* e = (fmi10::Element*)md;
    std::cout<<fmi10::elmNames[e->type]<<std::endl;
    for (int i = 0; i < e->n; i += 2) {
      std::cout<<e->attributes[i]<<"="<<e->attributes[i+1]<<std::endl;
    }
  }

  void* FMU10MExchObject::getFunctionAddress(const char* functionName)
  {
    std::string name = std::string(fmi10::getModelIdentifier(_ModelUnit_po->modelDescription)) + "_" + functionName;
  #if WINDOWS
      void* functionPointer = GetProcAddress(_ModelUnit_po->dllHandle, name.c_str());
  #else /* WINDOWS */
      void* functionPointer = dlsym(_ModelUnit_po->dllHandle, name.c_str());
  #endif /* WINDOWS */
      if (!functionPointer) {
  #if WINDOWS
        throw std::runtime_error(std::string("warning: Function") + name + " not found in dll");
  #else /* WINDOWS */
        throw std::runtime_error(std::string("warning: Function") + name
                                 + " not found in dll! Code = " + dlerror());
  #endif /* WINDOWS */
      }
      return functionPointer;
  }


} // namespace fmuw


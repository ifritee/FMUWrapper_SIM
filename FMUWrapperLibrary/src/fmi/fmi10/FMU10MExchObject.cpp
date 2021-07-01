#include <iostream>
#include <stdexcept>

#ifdef _WINDOWS
  #include <windows.h>
#else
  #include <dlfcn.h>
#endif

#include "FMU10MExchObject.h"
#include "Cconstants.h"
#include "xml_parser10.h"
#include "fmi_me.h"
#include "CMEData.h"

namespace fmuw
{

  FMU10MExchObject::FMU10MExchObject(const std::string &path)
    : FMU10ObjectAbstract(path)
    , _ModelUnit_po(new fmi10::FMU)
  {

  }

  FMU10MExchObject::~FMU10MExchObject()
  {
    if (_ModelData_po) {
      _ModelData_po->freeData();
    }
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
    HMODULE handle = LoadLibraryA(_FilenameDLL_str.c_str());
#else /* WINDOWS */
    HMODULE handle = dlopen(_FilenameDLL_str.c_str(), RTLD_LAZY);
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

  void FMU10MExchObject::initialize(double endTime, double stepSize)
  {
    _StepSize_d = stepSize;
    if (_ModelData_po != nullptr) {
      delete _ModelData_po;
    }
    _ModelData_po = new fmi10::CMEData(fmi10::getString(_ModelUnit_po->modelDescription, fmi10::att_guid), _ModelUnit_po);
    _ModelData_po->createComponent(/*calloc, free*/);
    _ModelData_po->Initialize_v(0.0, endTime);
  }

  void FMU10MExchObject::step()
  {
    _ModelData_po->step(_StepSize_d);
  }

  const std::map<std::string, int> & FMU10MExchObject::inputVariables()
  {
    return _ModelData_po->inVarNames();
  }

  const std::map<std::string, int> & FMU10MExchObject::outputVariables()
  {
    return _ModelData_po->outVarNames();
  }

  double FMU10MExchObject::doubleValue(const std::string & name)
  {
    return _ModelData_po->doubleVar(name);
  }

  bool FMU10MExchObject::boolValue(const std::string & name)
  {
    return _ModelData_po->boolVar(name);
  }

  int FMU10MExchObject::intValue(const std::string & name)
  {
    return _ModelData_po->intVar(name);
  }

  std::string FMU10MExchObject::strValue(const std::string & name)
  {
    return _ModelData_po->stringVar(name);
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


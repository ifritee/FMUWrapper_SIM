#include <iostream>
#include <stdexcept>

#ifdef WINDOWS
  #include <windows.h>
#else
  #include <dlfcn.h>
#endif

#include "FMU10CoSimObject.h"
#include "Cconstants.h"
#include "xml_parser10.h"
#include "fmi_cs.h"

namespace fmuw
{

  FMU10CoSimObject::FMU10CoSimObject(const std::string & path)
    : FMU10ObjectAbstract(path)
    , _ModelUnit_po(new fmi10::FMU)
  {

  }

  FMU10CoSimObject::~FMU10CoSimObject()
  {
#if WINDOWS
    FreeLibrary(_ModelUnit_po->dllHandle);
#else /* WINDOWS */
    dlclose(_ModelUnit_po->dllHandle);
#endif /* WINDOWS */
    delete _ModelUnit_po;
  }

  void FMU10CoSimObject::parse(std::string &fileName)
  {
    _ModelUnit_po->modelDescription = fmi10::parse(fileName.c_str());
    if (!_ModelUnit_po->modelDescription) {
      throw std::runtime_error("Model descriptor is crash");
    }
    printModelDescription(_ModelUnit_po->modelDescription);
    _FilenameDLL_str = _PathToFMU_str + "/" + LIBRARY_PATH
        + fmi10::getModelIdentifier(_ModelUnit_po->modelDescription) + LIBRARY_SUFFIX;
  }

  void FMU10CoSimObject::loadLibrary()
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
    _ModelUnit_po->instantiateSlave        = (fmi10::fInstantiateSlave)   getFunctionAddress("fmiInstantiateSlave");
    _ModelUnit_po->initializeSlave         = (fmi10::fInitializeSlave)    getFunctionAddress("fmiInitializeSlave");
    _ModelUnit_po->terminateSlave          = (fmi10::fTerminateSlave)     getFunctionAddress("fmiTerminateSlave");
    _ModelUnit_po->resetSlave              = (fmi10::fResetSlave)         getFunctionAddress("fmiResetSlave");
    _ModelUnit_po->freeSlaveInstance       = (fmi10::fFreeSlaveInstance)  getFunctionAddress("fmiFreeSlaveInstance");
    _ModelUnit_po->setRealInputDerivatives = (fmi10::fSetRealInputDerivatives) getFunctionAddress("fmiSetRealInputDerivatives");
    _ModelUnit_po->getRealOutputDerivatives = (fmi10::fGetRealOutputDerivatives) getFunctionAddress("fmiGetRealOutputDerivatives");
    _ModelUnit_po->cancelStep              = (fmi10::fCancelStep)         getFunctionAddress("fmiCancelStep");
    _ModelUnit_po->doStep                  = (fmi10::fDoStep)             getFunctionAddress("fmiDoStep");
    // SimulationX 3.4 and 3.5 do not yet export getStatus and getXStatus: do not count this as failure here
    _ModelUnit_po->getStatus               = (fmi10::fGetStatus)          getFunctionAddress("fmiGetStatus");
    _ModelUnit_po->getRealStatus           = (fmi10::fGetRealStatus)      getFunctionAddress("fmiGetRealStatus");
    _ModelUnit_po->getIntegerStatus        = (fmi10::fGetIntegerStatus)   getFunctionAddress("fmiGetIntegerStatus");
    _ModelUnit_po->getBooleanStatus        = (fmi10::fGetBooleanStatus)   getFunctionAddress("fmiGetBooleanStatus");
    _ModelUnit_po->getStringStatus         = (fmi10::fGetStringStatus)    getFunctionAddress("fmiGetStringStatus");
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

  void FMU10CoSimObject::printModelDescription(fmi10::ModelDescription *md)
  {
    fmi10::Element* e = (fmi10::Element*)md;
    std::cout<<fmi10::elmNames[e->type]<<std::endl;
    for (int i = 0; i < e->n; i += 2) {
      std::cout<<e->attributes[i]<<"="<<e->attributes[i+1]<<std::endl;
    }
  }

  void *FMU10CoSimObject::getFunctionAddress(const char *functionName)
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

#include <stdexcept>

#include "fmi2.h"
#include "string.h"
#include "CCS20Data.h"
#include "Cconstants.h"
#include "FMU20CoSimObject.h"

namespace fmuw
{

  FMU20CoSimObject::FMU20CoSimObject(const std::string & path)
    : FMU10ObjectAbstract(path)
    , _ModelUnit_po(new fmi20::FMU)
  {

  }

  FMU20CoSimObject::~FMU20CoSimObject()
  {
    if (_ModelData_po) {
      _ModelData_po->freeData();
    }
#if WINDOWS
    FreeLibrary(_ModelUnit_po->dllHandle);
#else /* WINDOWS */
    dlclose(_ModelUnit_po->dllHandle);
#endif /* WINDOWS */
    delete _ModelUnit_po;
    delete _ModelData_po;
  }

  void FMU20CoSimObject::parse(std::string &fileName)
  {
    char buffer[1024];
    strcpy(buffer, fileName.c_str());
    _ModelUnit_po->modelDescription = ::parse(buffer);
    if (!_ModelUnit_po->modelDescription) {
      throw std::runtime_error("Model descriptor is crash");
    }
    printModelDescription(_ModelUnit_po->modelDescription);
    const char * modelId = getAttributeValue((Element *)getCoSimulation(_ModelUnit_po->modelDescription), att_modelIdentifier);
    _FilenameDLL_str = _PathToFMU_str + "/" + LIBRARY_PATH + modelId + LIBRARY_SUFFIX;
  }

  void FMU20CoSimObject::loadLibrary()
  {
#if WINDOWS
    HMODULE handle = LoadLibraryA(_FilenameDLL_str.c_str());
#else /* WINDOWS */
    HMODULE handle = dlopen(_FilenameDLL_str.c_str(), RTLD_LAZY);
#endif /* WINDOWS */
    if (!handle) {
#if WINDOWS
      throw std::runtime_error(std::string("Model library was not load: ") + _FilenameDLL_str + " >>" + std::to_string(GetLastError()));
#else
      throw std::runtime_error(std::string("Model library was not load: ") + _FilenameDLL_str + " >>" + dlerror());
#endif
    }
    _ModelUnit_po->dllHandle = handle;
    //----- Загрузка методов -----
    _ModelUnit_po->getTypesPlatform         = (fmi2GetTypesPlatformTYPE *)          getFunctionAddress("fmi2GetTypesPlatform");
    _ModelUnit_po->getVersion               = (fmi2GetVersionTYPE *)                getFunctionAddress("fmi2GetVersion");
    _ModelUnit_po->setDebugLogging          = (fmi2SetDebugLoggingTYPE *)           getFunctionAddress("fmi2SetDebugLogging");
    _ModelUnit_po->instantiate              = (fmi2InstantiateTYPE *)               getFunctionAddress("fmi2Instantiate");
    _ModelUnit_po->freeInstance             = (fmi2FreeInstanceTYPE *)              getFunctionAddress("fmi2FreeInstance");
    _ModelUnit_po->setupExperiment          = (fmi2SetupExperimentTYPE *)           getFunctionAddress("fmi2SetupExperiment");
    _ModelUnit_po->enterInitializationMode  = (fmi2EnterInitializationModeTYPE *)   getFunctionAddress("fmi2EnterInitializationMode");
    _ModelUnit_po->exitInitializationMode   = (fmi2ExitInitializationModeTYPE *)    getFunctionAddress("fmi2ExitInitializationMode");
    _ModelUnit_po->terminate                = (fmi2TerminateTYPE *)                 getFunctionAddress("fmi2Terminate");
    _ModelUnit_po->reset                    = (fmi2ResetTYPE *)                     getFunctionAddress("fmi2Reset");
    _ModelUnit_po->getReal                  = (fmi2GetRealTYPE *)                   getFunctionAddress("fmi2GetReal");
    _ModelUnit_po->getInteger               = (fmi2GetIntegerTYPE *)                getFunctionAddress("fmi2GetInteger");
    _ModelUnit_po->getBoolean               = (fmi2GetBooleanTYPE *)                getFunctionAddress("fmi2GetBoolean");
    _ModelUnit_po->getString                = (fmi2GetStringTYPE *)                 getFunctionAddress("fmi2GetString");
    _ModelUnit_po->setReal                  = (fmi2SetRealTYPE *)                   getFunctionAddress("fmi2SetReal");
    _ModelUnit_po->setInteger               = (fmi2SetIntegerTYPE *)                getFunctionAddress("fmi2SetInteger");
    _ModelUnit_po->setBoolean               = (fmi2SetBooleanTYPE *)                getFunctionAddress("fmi2SetBoolean");
    _ModelUnit_po->setString                = (fmi2SetStringTYPE *)                 getFunctionAddress("fmi2SetString");
    _ModelUnit_po->getFMUstate              = (fmi2GetFMUstateTYPE *)               getFunctionAddress("fmi2GetFMUstate");
    _ModelUnit_po->setFMUstate              = (fmi2SetFMUstateTYPE *)               getFunctionAddress("fmi2SetFMUstate");
    _ModelUnit_po->freeFMUstate             = (fmi2FreeFMUstateTYPE *)              getFunctionAddress("fmi2FreeFMUstate");
    _ModelUnit_po->serializedFMUstateSize   = (fmi2SerializedFMUstateSizeTYPE *)    getFunctionAddress("fmi2SerializedFMUstateSize");
    _ModelUnit_po->serializeFMUstate        = (fmi2SerializeFMUstateTYPE *)         getFunctionAddress("fmi2SerializeFMUstate");
    _ModelUnit_po->deSerializeFMUstate      = (fmi2DeSerializeFMUstateTYPE *)       getFunctionAddress("fmi2DeSerializeFMUstate");
    _ModelUnit_po->getDirectionalDerivative = (fmi2GetDirectionalDerivativeTYPE *)  getFunctionAddress("fmi2GetDirectionalDerivative");
    _ModelUnit_po->setRealInputDerivatives  = (fmi2SetRealInputDerivativesTYPE *)   getFunctionAddress("fmi2SetRealInputDerivatives");
    _ModelUnit_po->getRealOutputDerivatives = (fmi2GetRealOutputDerivativesTYPE *)  getFunctionAddress("fmi2GetRealOutputDerivatives");
    _ModelUnit_po->doStep                   = (fmi2DoStepTYPE *)                    getFunctionAddress("fmi2DoStep");
    _ModelUnit_po->cancelStep               = (fmi2CancelStepTYPE *)                getFunctionAddress("fmi2CancelStep");
    _ModelUnit_po->getStatus                = (fmi2GetStatusTYPE *)                 getFunctionAddress("fmi2GetStatus");
    _ModelUnit_po->getRealStatus            = (fmi2GetRealStatusTYPE *)             getFunctionAddress("fmi2GetRealStatus");
    _ModelUnit_po->getIntegerStatus         = (fmi2GetIntegerStatusTYPE *)          getFunctionAddress("fmi2GetIntegerStatus");
    _ModelUnit_po->getBooleanStatus         = (fmi2GetBooleanStatusTYPE *)          getFunctionAddress("fmi2GetBooleanStatus");
    _ModelUnit_po->getStringStatus          = (fmi2GetStringStatusTYPE *)           getFunctionAddress("fmi2GetStringStatus");
  }

  void FMU20CoSimObject::initialize(double endTime, double stepSize)
  {
    _StepSize_d = stepSize;
    if (_ModelData_po != nullptr) {
      delete _ModelData_po;
    }
    _ModelData_po = new fmi20::CCS20Data(_ModelUnit_po, _PathToFMU_str);
    _ModelData_po->createComponent(/*calloc, free*/);
    _ModelData_po->Initialize_v(0.0, endTime);
  }

  void FMU20CoSimObject::step()
  {
    _ModelData_po->step(_StepSize_d);
  }

  const std::map<std::string, int> &FMU20CoSimObject::inputVariables()
  {
    return _ModelData_po->inVarNames();
  }

  const std::map<std::string, int> &FMU20CoSimObject::outputVariables()
  {
    return _ModelData_po->outVarNames();
  }

  double FMU20CoSimObject::doubleValue(const std::string & name)
  {
    return _ModelData_po->doubleVar(name);
  }

  bool FMU20CoSimObject::boolValue(const std::string & name)
  {
    return _ModelData_po->boolVar(name);
  }

  int FMU20CoSimObject::intValue(const std::string & name)
  {
    return _ModelData_po->intVar(name);
  }

  std::string FMU20CoSimObject::strValue(const std::string & name)
  {
    return _ModelData_po->stringVar(name);
  }

  void FMU20CoSimObject::setDoubleValue(const std::string & name, double value)
  {
    _ModelData_po->setRealData(name, &value);
  }

  void FMU20CoSimObject::setBoolValue(const std::string & name, bool value)
  {
    int temp = static_cast<int>(value);
    _ModelData_po->setBoolData(name, &temp);
  }

  void FMU20CoSimObject::setIntValue(const std::string & name, int value)
  {
    _ModelData_po->setIntData(name, &value);
  }

  void FMU20CoSimObject::setStringValue(const std::string & name, const std::string & value)
  {
    const char * strings[] = {value.c_str()};
    _ModelData_po->setStrData(name, strings);
  }

  void FMU20CoSimObject::printModelDescription(ModelDescription* md)
  {
//    Element* e = (Element*)md;
//    int number; // number of attributes
//    const char **attributes = getAttributesAsArray(e, &number);
//    Component *component;

//    if (!attributes) {
//      printf("ModelDescription printing aborted.");
//      return;
//    }
//    printf("%s\n", getElementTypeName(e));
//    for (int i = 0; i < number; i += 2) {
//      printf("  %s=%s\n", attributes[i], attributes[i+1]);
//    }
//    free((void *)attributes);
//    component = getModelExchange(md);
//    if (!component) {
//      throw std::runtime_error("No ModelExchange element found in model description. This FMU is not for Model Exchange.");
//    }
//    printf("%s\n", getElementTypeName((Element *)component));
//    attributes = getAttributesAsArray((Element *)component, &number);
//    if (!attributes) {
//      printf("ModelDescription printing aborted.");
//      return;
//    }
//    for (int i = 0; i < number; i += 2) {
//      printf("  %s=%s\n", attributes[i], attributes[i+1]);
//    }

//    free((void *)attributes);
  }

  void* FMU20CoSimObject::getFunctionAddress(const char* functionName)
  {
#if WINDOWS
    void* functionPointer = GetProcAddress(_ModelUnit_po->dllHandle, functionName);
#else /* WINDOWS */
    void* functionPointer = dlsym(_ModelUnit_po->dllHandle, functionName);
#endif /* WINDOWS */
    if (!functionPointer) {
#if WINDOWS
      throw std::runtime_error(std::string("warning: Function") + functionName + " not found in dll");
#else /* WINDOWS */
      throw std::runtime_error(std::string("warning: Function") + functionName + " not found in dll! Code = " + dlerror());
#endif /* WINDOWS */
    }
    return functionPointer;
  }

} // namespace fmuw

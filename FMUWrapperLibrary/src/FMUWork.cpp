#include <fstream>

#include "FMUWork.h"
#include "Cconstants.h"
#include "FMU10Object.h"
#include "FMU20Object.h"

namespace fmuw
{

  FMUWork::FMUWork(const std::string & path)
    : m_fmuPath(path)
  {

  }

  FMUWork::~FMUWork()
  {
    delete m_fmuObject;
    m_fmuObject = nullptr;
  }

  bool FMUWork::DescriptionRead_b()
  {
    std::ifstream stream;
    std::string fileName = m_fmuPath + "/" + DESRIPTION_FILE;
    stream.open(fileName);
    if (!stream.is_open()) {
      m_lastError = "File XML not fount: " + m_fmuPath + "/" + DESRIPTION_FILE;
      return false;
    }
    try {
      boost::property_tree::ptree propertyTree;
      boost::property_tree::read_xml(stream, propertyTree);
      m_fmuVersion = propertyTree.get<std::string>("fmiModelDescription.<xmlattr>.fmiVersion");
      m_modelName = propertyTree.get<std::string>("fmiModelDescription.<xmlattr>.modelName");
      if (m_fmuObject) {
        delete m_fmuObject;
        m_fmuObject = nullptr;
      }
      if (m_fmuVersion == "1.0") {
        m_fmuObject = new FMU10Object(m_fmuPath);
      } else if (m_fmuVersion == "2.0") {
        m_fmuObject = new FMU20Object(m_fmuPath);
      } else {
        m_lastError = "FMU version is not valid = " + m_fmuVersion;
        return false;
      }
      m_fmuObject->parse(propertyTree);
    } catch(boost::property_tree::xml_parser_error &e) {
      m_lastError = DESRIPTION_FILE + std::string(" parsing is crashed: ") + e.what();
      stream.close();
      return false;
    }
    stream.close();
    return true;
  }

  std::string FMUWork::fmuPath() const
  {
    return m_fmuPath;
  }

  std::string FMUWork::lastError() const
  {
    return m_lastError;
  }

  void FMUWork::modelInit(double entTime, double stepSize)
  {
    if (m_fmuObject) {
      m_fmuObject->initialize(entTime, stepSize);
    }
  }

  int FMUWork::inputsQty()
  {
    return m_fmuObject->inputVariables().size();
  }

  int FMUWork::outputsQty()
  {
    return m_fmuObject->outputVariables().size();
  }

  void FMUWork::modelStep()
  {
    if (m_fmuObject) {
      m_fmuObject->step();
    }
  }

  std::string FMUWork::outputVar(int index, int &type)
  {
    std::map<std::string, int> vars = m_fmuObject->outputVariables();
    int count = 0;
    std::string name;
    for (std::map<std::string, int>::iterator It = vars.begin(); It != vars.end(); ++It) {
      if (count == index) {
        type = It->second;
        name = It->first;
        break;
      }
      ++count;
    }
    return name;
  }

  std::string FMUWork::inputVar(int index, int &type)
  {
    std::map<std::string, int> vars = m_fmuObject->inputVariables();
    int count = 0;
    std::string name;
    for (std::map<std::string, int>::iterator It = vars.begin(); It != vars.end(); ++It) {
      if (count == index) {
        type = It->second;
        name = It->first;
        break;
      }
      ++count;
    }
    return name;
  }

  double FMUWork::doubleValue(const std::string & name)
  {
    return m_fmuObject->doubleValue(name);
  }

  bool FMUWork::boolValue(const std::string & name)
  {
    return m_fmuObject->boolValue(name);
  }

  int FMUWork::intValue(const std::string & name)
  {
    return m_fmuObject->intValue(name);
  }

  std::string FMUWork::strValue(const std::string & name)
  {
    return m_fmuObject->strValue(name);
  }

  void FMUWork::setDoubleValue(const char *name, double value)
  {
    m_fmuObject->setDoubleValue(name, value);
  }

  void FMUWork::setBoolValue(const char *name, bool value)
  {
    m_fmuObject->setBoolValue(name, value);
  }

  void FMUWork::setIntValue(const char *name, int value)
  {
    m_fmuObject->setIntValue(name, value);
  }

  void FMUWork::setStringValue(const char *name, std::string value)
  {
    m_fmuObject->setStringValue(name, value);
  }

} // namespace fmuw


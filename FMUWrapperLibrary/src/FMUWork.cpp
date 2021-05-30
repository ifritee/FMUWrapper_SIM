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

} // namespace fmuw


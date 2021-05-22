#include <fstream>
#include <boost/property_tree/xml_parser.hpp>

#include "FMUWork.h"

namespace fmuw
{

  FMUWork::FMUWork(const std::string & path)
    : m_fmuPath(path)
  {

  }

  FMUWork::~FMUWork()
  {

  }

  bool FMUWork::DescriptionRead_b()
  {
    std::ifstream stream;
    stream.open(m_fmuPath + "/" + DESRIPTION_FILE);
    if (!stream.is_open()) {
      m_lastError = "File XML not fount: " + m_fmuPath + "/" + DESRIPTION_FILE;
      return false;
    }
    try {
      boost::property_tree::ptree propertyTree;
      boost::property_tree::read_xml(stream, propertyTree);
      m_fmuVersion = propertyTree.get<std::string>("fmiModelDescription.<xmlattr>.fmiVersion");
      m_modelName = propertyTree.get<std::string>("fmiModelDescription.<xmlattr>.modelName");

    } catch(boost::property_tree::xml_parser_error &e) {
      m_lastError = DESRIPTION_FILE + " parsing is crashed: " + e.what();
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


#include <iostream>

#include "FMU20Object.h"
#include "FMU20CoSimObject.h"
#include "FMU20MExchObject.h"

namespace fmuw
{

  FMU20Object::FMU20Object(const std::string & path)
    : FMUAbstract(path)
  {

  }

  FMU20Object::~FMU20Object()
  {
    delete _Model_po;
  }

  void FMU20Object::parse(boost::property_tree::ptree &tree)
  {
    if (_Model_po) {
      delete _Model_po;
    }
    // Нужно уточнить, что это exchenge model или co-simulator
    try {
      boost::property_tree::ptree imp = tree.get_child("fmiModelDescription.ModelExchange");
      _Model_po = new FMU20MExchObject(_FMUPath_str);

      std::cout<<"Model exchange simulator"<<std::endl;
    } catch(...) {
      try {
        boost::property_tree::ptree imp = tree.get_child("fmiModelDescription.CoSimulation");
        _Model_po = new FMU20CoSimObject(_FMUPath_str);
        std::cout<<"Co-Simul simulator"<<std::endl;

      } catch(...) {
        return;
      }
    }

    _Model_po->parse(_XMLFilename_str);
    _Model_po->loadLibrary();
  }

  void FMU20Object::initialize(double entTime, double stepSize)
  {
    _Model_po->initialize(entTime, stepSize);
  }

  void FMU20Object::step()
  {
    _Model_po->step();
  }

  const std::map<std::string, int> &FMU20Object::inputVariables()
  {
    return _Model_po->inputVariables();
  }

  const std::map<std::string, int> &FMU20Object::outputVariables()
  {
    return _Model_po->outputVariables();
  }

  double FMU20Object::doubleValue(const std::string & name)
  {
    return _Model_po->doubleValue(name);
  }

  bool FMU20Object::boolValue(const std::string & name)
  {
    return _Model_po->boolValue(name);
  }

  int FMU20Object::intValue(const std::string & name)
  {
    return _Model_po->intValue(name);
  }

  std::string FMU20Object::strValue(const std::string & name)
  {
    return _Model_po->strValue(name);
  }

} // namespace fmuw

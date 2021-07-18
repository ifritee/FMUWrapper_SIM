#include <iostream>

#include "FMU10ObjectAbstract.h"
#include "FMU10Object.h"
#include "FMU10CoSimObject.h"
#include "FMU10MExchObject.h"

namespace fmuw
{

  FMU10Object::FMU10Object(const std::string &path)
    : FMUAbstract(path)
  {

  }

  FMU10Object::~FMU10Object()
  {
    delete _Model_po;
  }

  void FMU10Object::parse(boost::property_tree::ptree &tree)
  {
    if (_Model_po) {
      delete _Model_po;
    }
    // Нужно уточнить, что это exchenge model или co-simulator
    try {
      boost::property_tree::ptree imp = tree.get_child("fmiModelDescription.Implementation.CoSimulation_StandAlone");
      _Model_po = new FMU10CoSimObject(_FMUPath_str);
      std::cout<<"CO simulator"<<std::endl;
    } catch(...) {
      _Model_po = new FMU10MExchObject(_FMUPath_str);
    }

    _Model_po->parse(_XMLFilename_str);
    _Model_po->loadLibrary();
  }

  void FMU10Object::initialize(double entTime, double stepSize)
  {
    _Model_po->initialize(entTime, stepSize);
  }

  void FMU10Object::step()
  {
    _Model_po->step();
  }

  const std::map<std::string, int> &FMU10Object::inputVariables()
  {
    return _Model_po->inputVariables();
  }

  const std::map<std::string, int> &FMU10Object::outputVariables()
  {
    return _Model_po->outputVariables();
  }

  double FMU10Object::doubleValue(const std::string & name)
  {
    return _Model_po->doubleValue(name);
  }

  bool FMU10Object::boolValue(const std::string & name)
  {
    return _Model_po->boolValue(name);
  }

  int FMU10Object::intValue(const std::string & name)
  {
    return _Model_po->intValue(name);
  }

  std::string FMU10Object::strValue(const std::string & name)
  {
    return _Model_po->strValue(name);
  }

  void FMU10Object::setDoubleValue(const std::string & name, double value)
  {
    _Model_po->setDoubleValue(name, value);
  }

  void FMU10Object::setBoolValue(const std::string & name, bool value)
  {
    _Model_po->setBoolValue(name, value);
  }

  void FMU10Object::setIntValue(const std::string & name, int value)
  {
    _Model_po->setIntValue(name, value);
  }

  void FMU10Object::setStringValue(const std::string & name, const std::string & value)
  {
    _Model_po->setStringValue(name, value);
  }

} // namespace fmuw

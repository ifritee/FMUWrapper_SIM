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

  void FMU10Object::initialize(double entTime)
  {
    _Model_po->initialize(entTime);
  }

} // namespace fmuw

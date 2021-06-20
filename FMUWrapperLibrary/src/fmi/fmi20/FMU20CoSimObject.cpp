#include "FMU20CoSimObject.h"

namespace fmuw
{

  FMU20CoSimObject::FMU20CoSimObject(const std::string & path)
    : FMU10ObjectAbstract(path)
  {

  }

  FMU20CoSimObject::~FMU20CoSimObject()
  {

  }

  void FMU20CoSimObject::parse(std::string &fileName)
  {

  }

  void FMU20CoSimObject::loadLibrary()
  {

  }

  void FMU20CoSimObject::initialize(double endTime, double stepSize)
  {

  }

  void FMU20CoSimObject::step()
  {

  }

  const std::map<std::string, int> &FMU20CoSimObject::inputVariables()
  {

  }

  const std::map<std::string, int> &FMU20CoSimObject::outputVariables()
  {

  }

  double FMU20CoSimObject::doubleValue(const std::string &)
  {

  }

  bool FMU20CoSimObject::boolValue(const std::string &)
  {

  }

  int FMU20CoSimObject::intValue(const std::string &)
  {

  }

  std::string FMU20CoSimObject::strValue(const std::string &)
  {

  }

} // namespace fmuw

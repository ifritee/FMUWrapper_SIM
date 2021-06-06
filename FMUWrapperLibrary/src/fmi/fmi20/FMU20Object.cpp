#include "FMU20Object.h"

namespace fmuw
{

  FMU20Object::FMU20Object(const std::string & path)
    : FMUAbstract(path)
  {

  }

  FMU20Object::~FMU20Object()
  {

  }

  void FMU20Object::parse(boost::property_tree::ptree &tree)
  {

  }

  void FMU20Object::initialize(double entTime, double stepSize)
  {

  }

  void FMU20Object::step()
  {

  }

  const std::map<std::string, int> &FMU20Object::inputVariables()
  {
    return std::map<std::string, int>();
  }

  const std::map<std::string, int> &FMU20Object::outputVariables()
  {
    return std::map<std::string, int>();
  }

  double FMU20Object::doubleValue(const std::string &)
  {
    return 0.0;
  }

} // namespace fmuw

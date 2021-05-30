#include "FMUAbstract.h"
#include "Cconstants.h"

namespace fmuw
{
  FMUAbstract::FMUAbstract(const std::string & path)
    : _FMUPath_str(path)
    , _XMLFilename_str(path +  + "/" + DESRIPTION_FILE)
  {

  }

  FMUAbstract::~FMUAbstract()
  {

  }

} // namespace fmuw

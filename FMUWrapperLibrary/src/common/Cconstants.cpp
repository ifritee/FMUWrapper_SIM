#include <stdexcept>
#include <stdarg.h>
#include <iostream>

#include "Cconstants.h"

namespace fmuw
{

  void fmuLogger(fmiComponent c, fmiString instanceName, fmiStatus status, fmiString category, fmiString message, ...) {
    char MessageBuffer_ac[1024];
    va_list argp;
    va_start(argp, message);
    vsprintf(MessageBuffer_ac, message, argp);
    va_end(argp);

    std::string Status_str;
    switch (status) {
      case fmiOK:      Status_str = "ok";
      case fmiWarning: Status_str = "warning";
      case fmiDiscard: Status_str = "discard";
      case fmiError:   Status_str = "error";
      case fmiFatal:   Status_str = "fatal";
    }
    if (status == fmiError || status == fmiFatal) {
      throw std::logic_error(Status_str + " >> " + instanceName + category + MessageBuffer_ac);
    }
  }

} // namespace fmuw

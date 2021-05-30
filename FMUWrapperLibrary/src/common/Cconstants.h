#ifndef CCONSTANTS_H
#define CCONSTANTS_H

#include <string>

#if WINDOWS
  #ifdef _WIN64
    #define DLL_DIR   "binaries\\win64\\"
  #else /* _WIN64 */
    #define DLL_DIR   "binaries\\win32\\"
  #endif /* _WIN64 */

  #define DLL_SUFFIX ".dll"
#elif __APPLE__
  // Use these for platforms other than OpenModelica
  #define DLL_DIR   "binaries/darwin64/"
  #define DLL_SUFFIX ".dylib"
#elif __linux__
  #ifdef __x86_64__
    #define DLL_DIR   "binaries/linux64/"
  #else
    // It may be necessary to compile with -m32, see ../Makefile
    #define DLL_DIR   "binaries/linux32/"
  #endif /*__x86_64__*/
  #define DLL_SUFFIX ".so"
#else
  #error "OS type is not supported!!!"
#endif

namespace fmuw
{
  constexpr const char * DESRIPTION_FILE = "modelDescription.xml";
  constexpr const char * LIBRARY_PATH = DLL_DIR;
  constexpr const char * LIBRARY_SUFFIX = DLL_SUFFIX;
} // namespace fmuw

#endif // CCONSTANTS_H

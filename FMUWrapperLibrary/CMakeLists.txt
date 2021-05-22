cmake_minimum_required(VERSION 3.5)

project(FMUWrapperLibrary LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
SET(BOOST_REQUIRED_COMPONENTS)

set(SOURCE_FILES
  src/main.cpp
  src/libfmuwrapper.cpp
  src/FMUWork.cpp
  src/fmi/FMUAbstract.cpp
  src/fmi/fmi10/FMU10Object.cpp
  src/fmi/fmi10/shared/parser/stack.c
  src/fmi/fmi10/shared/parser/xml_parser.c
  src/fmi/fmi20/FMU20Object.cpp
  src/fmi/fmi20/parser/XmlElement.cpp
  src/fmi/fmi20/parser/XmlParser.cpp
  src/fmi/fmi20/parser/XmlParserCApi.cpp
  )

set(HEADER_FILES
  src/libfmuwrapper.h
  src/libfmuwrapper_global.h
  src/FMUWork.h
  src/fmi/FMUAbstract.h
  src/fmi/fmi10/FMU10Object.h
  src/fmi/fmi10/shared/parser/stack.h
  src/fmi/fmi10/shared/parser/xml_parser.h
  src/fmi/fmi10/shared/parser/expat_external.h
  src/fmi/fmi10/shared/parser/expat.h
  src/fmi/fmi10/shared/fmi_cs.h
  src/fmi/fmi10/shared/fmi_me.h
  src/fmi/fmi10/shared/include/fmiFunctions.h
  src/fmi/fmi10/shared/include/fmiModelFunctions.h
  src/fmi/fmi10/shared/include/fmiModelTypes.h
  src/fmi/fmi10/shared/include/fmiPlatformTypes.h
  src/fmi/fmi20/FMU20Object.h
  src/fmi/fmi20/parser/XmlElement.h
  src/fmi/fmi20/parser/XmlParser.h
  src/fmi/fmi20/parser/XmlParserCApi.h
  src/fmi/fmi20/parser/XmlParserException.h
  src/fmi/fmi20/include/fmi2Functions.h
  src/fmi/fmi20/include/fmi2FunctionTypes.h
  src/fmi/fmi20/include/fmi2TypesPlatform.h
  third_party/libzip/lib/zip.h
  )

include_directories(
  src
  src/fmi
  src/fmi/fmi10
  src/fmi/fmi10/shared/include
  src/fmi/fmi10/shared/parser
  src/fmi/fmi20/
  src/fmi/fmi20/include
  src/fmi/fmi20/parser
  src/common
  third_party/libzip/lib
  )

add_definitions(
  -D__WITH_LIBRARY__
  -DFMUWRAPPER_LIBRARY
  )

IF (CMAKE_BUILD_TYPE MATCHES Debug)
    message(">>>>>> DEBUG MODE <<<<<<")
    add_definitions(-DDEBUG_MODE=1 -D_LANG_RU_)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    message(">>>>>> RELEASE MODE <<<<<<")
    add_definitions(-DDEBUG_MODE=0)
ENDIF ()

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/third_party/libzip/build/lib/libzip.a)

find_package(Threads REQUIRED)
find_package(Boost 1.69 COMPONENTS program_options log log_setup REQUIRED)
find_package(ZLIB)

target_link_libraries(${PROJECT_NAME} ZLIB::ZLIB)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
else()
  message(FATAL_ERROR "!!! BOOST libraries NOT found !!!")
endif()


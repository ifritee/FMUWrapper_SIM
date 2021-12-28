cmake_minimum_required(VERSION 3.5)

project(FMUWrapperLibrary LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)

set(SOURCE_FILES
  src/main.cpp
  src/libfmuwrapper.cpp
  src/FMUWork.cpp
  src/common/Cconstants.cpp
  src/fmi/FMUAbstract.cpp
  src/fmi/fmi10/FMU10Object.cpp
  src/fmi/fmi10/FMU10CoSimObject.cpp
  src/fmi/fmi10/FMU10MExchObject.cpp
  src/fmi/fmi10/CMEData.cpp
  src/fmi/fmi10/CCSData.cpp
  src/fmi/fmi10/shared/parser/stack10.cpp
  src/fmi/fmi10/shared/parser/xml_parser10.cpp
  src/fmi/fmi20/FMU20Object.cpp
  src/fmi/fmi20/FMU20MExchObject.cpp
  src/fmi/fmi20/FMU20CoSimObject.cpp
  src/fmi/fmi20/CME20Data.cpp
  src/fmi/fmi20/CCS20Data.cpp
  src/fmi/fmi20/parser/XmlElement.cpp
  src/fmi/fmi20/parser/XmlParser.cpp
  src/fmi/fmi20/parser/XmlParserCApi.cpp
  )

set(HEADER_FILES
  src/libfmuwrapper.h
  src/libfmuwrapper_global.h
  src/FMUWork.h
  src/common/Cconstants.h
  src/fmi/FMUAbstract.h
  src/fmi/fmi10/FMU10Object.h
  src/fmi/fmi10/FMU10ObjectAbstract.h
  src/fmi/fmi10/FMU10CoSimObject.h
  src/fmi/fmi10/FMU10MExchObject.h
  src/fmi/fmi10/CMEData.h
  src/fmi/fmi10/CCSData.h
  src/fmi/fmi10/shared/parser/stack10.h
  src/fmi/fmi10/shared/parser/xml_parser10.h
  src/fmi/fmi10/shared/fmi_cs.h
  src/fmi/fmi10/shared/fmi_me.h
  src/fmi/fmi10/shared/include/fmiFunctions.h
  src/fmi/fmi10/shared/include/fmiModelFunctions.h
  src/fmi/fmi10/shared/include/fmiModelTypes.h
  src/fmi/fmi10/shared/include/fmiPlatformTypes.h
  src/fmi/fmi20/FMU20Object.h
  src/fmi/fmi20/FMU20MExchObject.h
  src/fmi/fmi20/FMU20CoSimObject.h
  src/fmi/fmi20/CME20Data.h
  src/fmi/fmi20/CCS20Data.h
  src/fmi/fmi20/parser/XmlParserCApi.h
  src/fmi/fmi20/parser/fmu20/XmlElement.h
  src/fmi/fmi20/parser/fmu20/XmlParser.h
  src/fmi/fmi20/parser/fmu20/XmlParserException.h
  src/fmi/fmi20/include/fmi2Functions.h
  src/fmi/fmi20/include/fmi2FunctionTypes.h
  src/fmi/fmi20/include/fmi2TypesPlatform.h
  )

include_directories(
  src
  src/fmi
  src/fmi/fmi10
  src/fmi/fmi10/shared
  src/fmi/fmi10/shared/include
  src/fmi/fmi10/shared/parser
  src/fmi/fmi20/
  src/fmi/fmi20/include
  src/fmi/fmi20/parser
  src/fmi/fmi20/parser/fmu20
  src/fmi/fmi20/parser/libxml
  src/common
  third_party/libzip/lib
  third_party/libexpat
  third_party
  )

add_definitions(
  -D__WITH_LIBRARY__
  -DFMUWRAPPER_LIBRARY
  -DSTANDALONE_XML_PARSER
  )

IF (CMAKE_BUILD_TYPE MATCHES Debug)
    message(">>>>>> DEBUG MODE <<<<<<")
    add_definitions(-DDEBUG_MODE=1 -D_LANG_RU_)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    message(">>>>>> RELEASE MODE <<<<<<")
    add_definitions(-DDEBUG_MODE=0)
ENDIF ()

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})
#target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/third_party/libzip/build/lib/libzip.a)
target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/third_party/libexpat/nix_64/libexpat.a)
target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})

find_package(Threads REQUIRED)
#find_package(ZLIB)
find_package(LibXml2 REQUIRED)

#target_link_libraries(${PROJECT_NAME} ZLIB::ZLIB)
target_link_libraries(${PROJECT_NAME} ${LIBXML2_LIBRARIES})

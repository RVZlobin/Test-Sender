CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
PROJECT(SerialDevice)

INCLUDE(GenerateExportHeader)

SET(GLOBAL PROPERTY USE_FOLDERS ON)

IF (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -pthread -Wall")
ELSEIF (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  # using Visual Studio C++
ENDIF()


FIND_PACKAGE(Boost REQUIRED COMPONENTS system)

SET(Boost_USE_STATIC_LIBS ON)

SET (SerialDevice_HEADERS 
  src/dev/device.h
  src/dev/serialdevice.h
  src/dev/serialdevicedriver.h
  src/core/protocol.h
  src/core/command.h
)

SET (SerialDevice_SOURCE
  src/dev/serialdevice.cpp
  src/dev/serialdevicedriver.cpp
  src/core/protocol.cpp
  src/core/command.cpp
)

ADD_LIBRARY(SerialDevice SHARED
  ${SerialDevice_HEADERS}
  ${SerialDevice_SOURCE}
)
  
TARGET_INCLUDE_DIRECTORIES(SerialDevice 
  PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/src/
          ${CMAKE_CURRENT_BINARY_DIR}
          ${Boost_INCLUDE_DIR}
)

TARGET_LINK_LIBRARIES(SerialDevice
  PUBLIC ${Boost_SYSTEM_LIBRARY} 
         ${Boost_LOG_LIBRARY}
)

GENERATE_EXPORT_HEADER(SerialDevice
  EXPORT_MACRO_NAME SERIALDEVICE_API)

SET_TARGET_PROPERTIES(SerialDevice PROPERTIES
  FOLDER "${CMAKE_PROJECT_NAME}/Serial Device Libraries"
  OUTPUT_NAME "SerialDevice")
  
TARGET_COMPILE_DEFINITIONS(SerialDevice 
  PRIVATE BOOST_ALL_NO_LIB
          BOOST_ALL_DYN_LINK
)

INSTALL(TARGETS SerialDevice RUNTIME DESTINATION bin)

cmake_minimum_required(VERSION 3.0)
PROJECT(testsender)

set(GLOBAL PROPERTY USE_FOLDERS ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -pthread -Wall -pedantic")

FIND_PACKAGE(Boost 1.69.0 REQUIRED COMPONENTS system log log_setup)

SET(Boost_USE_STATIC_LIBS ON)

SET (TestSender_HEADERS 
  src/dev/device.h
  src/dev/serialdevice.h
  src/dev/serialdevicedriver.h
  src/core/protocol.h
  src/core/command.h
)

SET (TestSender_SOURCE
  src/dev/serialdevice.cpp
  src/dev/serialdevicedriver.cpp
  src/core/protocol.cpp
  src/core/command.cpp
)

ADD_EXECUTABLE(testsender
  main.cpp
  ${TestSender_HEADERS}
  ${TestSender_SOURCE}
)

TARGET_INCLUDE_DIRECTORIES(testsender 
  PRIVATE ${CMAKE_SOURCE_DIR}/src/
          ${CMAKE_BINARY_DIR}/src/
          ${Boost_INCLUDE_DIR}
  
)

TARGET_LINK_LIBRARIES(testsender
  PRIVATE ${Boost_SYSTEM_LIBRARY} 
  PRIVATE ${Boost_LOG_LIBRARY}
)

TARGET_COMPILE_DEFINITIONS(testsender 
  PRIVATE BOOST_ALL_NO_LIB
          BOOST_ALL_DYN_LINK
)

INSTALL(TARGETS testsender RUNTIME DESTINATION bin)

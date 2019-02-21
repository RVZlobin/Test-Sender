cmake_minimum_required(VERSION 3.0)
PROJECT(testsender)

set(GLOBAL PROPERTY USE_FOLDERS ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -pthread -Wall")
FIND_PACKAGE(Boost REQUIRED)

SET(Boost_USE_STATIC_LIBS ON)

ADD_DEFINITIONS(
  -DBOOST_ALL_NO_LIB
  -DBOOST_ALL_DYN_LINK
)

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
          ${Boost_INCLUDEDIR}
  
)

TARGET_LINK_LIBRARIES(testsender
    PRIVATE ${Boost_SYSTEM_LIBRARY} 
    PRIVATE ${Boost_LOG_LIBRARY}
)

INSTALL(TARGETS testsender RUNTIME DESTINATION bin)

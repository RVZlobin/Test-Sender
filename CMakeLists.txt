cmake_minimum_required(VERSION 3.0)

project(testsender)

add_executable(testsender main.cpp)

install(TARGETS testsender RUNTIME DESTINATION bin)

cmake_minimum_required(VERSION 3.16.1)

project(demo_1)

include(CMakePrintHelpers)
cmake_print_variables(CMAKE_CXX_FLAGS_DEBUG)
cmake_print_variables(CMAKE_CXX_FLAGS_RELEASE)
cmake_print_variables(CMAKE_CXX_FLAGS_RELWITHDEBINF)


cmake_minimum_required(VERSION 3.26)
project(curve)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(thirdparty)
add_subdirectory(core)
add_subdirectory(jupyter/cpp)

cmake_minimum_required(VERSION 3.24)

project(demo)

set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)

add_subdirectory(src)
add_subdirectory(lib)


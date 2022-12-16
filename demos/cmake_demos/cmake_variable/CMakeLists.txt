cmake_minimum_required(VERSION 3.16)
project(demo)

add_subdirectory(foo)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tmp/bin) # this not apply to foo
add_subdirectory(bar) # bar has its own CMAKE_RUNTIME_OUTPUT_DIRECTORY, see bar/CMakeLists.txt
add_subdirectory(baz) # baz is affected


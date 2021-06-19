cmake_minimum_required(VERSION 3.16.1)

if(WIN32)
    cmake_policy(SET CMP0091 NEW)
    # use compile option /MD
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    # remove compile option "/RTC1" in default CMAKE_CXX_FLAGS_DEBUG 
    set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Ob0 /Od /MP")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP" )
endif()

project(hello_world VERSION 1.0.0 LANGUAGES CXX C)

set (CMAKE_CXX_STANDARD 11)

enable_testing()

add_subdirectory(src)
add_subdirectory(test)

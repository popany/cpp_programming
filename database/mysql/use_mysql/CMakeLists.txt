cmake_minimum_required (VERSION 3.15.7)

if(WIN32)
    cmake_policy(SET CMP0091 NEW)
    # use compile option /MD
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    # remove compile option "/RTC1" in default CMAKE_CXX_FLAGS_DEBUG
    set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Ob0 /Od")
endif()

project ("use_mysql")

if(WIN32)
    add_subdirectory(mysql_xDevApi)
endif()

add_subdirectory(mysql_accessor)


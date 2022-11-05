cmake_minimum_required(VERSION 3.16)

project(log_demo VERSION 1.0.0 LANGUAGES CXX C)

set (CMAKE_CXX_STANDARD 11)

add_subdirectory(log)

add_executable(log_demo main.cpp)

target_link_libraries(log_demo
    PRIVATE
        logger
)

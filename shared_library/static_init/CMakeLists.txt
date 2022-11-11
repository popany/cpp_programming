add_subdirectory(lib)

add_executable(run main.cpp)

target_link_libraries(run PUBLIC dl)

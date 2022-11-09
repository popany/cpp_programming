add_subdirectory(lib)

add_executable(run2 main.cpp)

target_link_libraries(run2 PUBLIC dl)


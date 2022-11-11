add_subdirectory(lib)

add_link_options(-rdynamic)
add_executable(run2 main.cpp)

target_link_libraries(run2 PUBLIC dl)


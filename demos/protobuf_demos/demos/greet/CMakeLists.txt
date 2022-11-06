add_subdirectory(proto)

add_executable(greet main.cpp)
target_link_libraries(greet PRIVATE greet_proto)

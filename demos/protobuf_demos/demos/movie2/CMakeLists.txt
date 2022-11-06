add_subdirectory(proto)

add_executable(movie2 main.cpp)
target_link_libraries(movie2 PRIVATE movie_proto2)

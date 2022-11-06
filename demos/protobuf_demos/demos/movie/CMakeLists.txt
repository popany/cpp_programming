add_subdirectory(proto)

add_executable(movie main.cpp)
target_link_libraries(movie PRIVATE movie_proto)

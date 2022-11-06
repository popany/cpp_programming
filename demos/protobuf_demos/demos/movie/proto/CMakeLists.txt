get_target_property(PROTOBUF_INSTALL_DIR protobuf "INSTALL_DIR")
include(CMakePrintHelpers)
cmake_print_variables(PROTOBUF_INSTALL_DIR)
set(PBUF_PROTOC ${PROTOBUF_INSTALL_DIR}/bin/protoc)

custom_protobuf_generate_cpp(MOVIE_PROTO_SRCS MOVIE_PROTO_HDRS movie.proto)
add_library(movie_proto STATIC ${MOVIE_PROTO_SRCS} ${MOVIE_PROTO_HDRS})
target_include_directories(movie_proto PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(movie_proto PUBLIC protobuf)

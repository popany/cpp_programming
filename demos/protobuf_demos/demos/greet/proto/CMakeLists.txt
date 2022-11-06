get_target_property(PROTOBUF_INSTALL_DIR protobuf "INSTALL_DIR")
include(CMakePrintHelpers)
cmake_print_variables(PROTOBUF_INSTALL_DIR)
set(PBUF_PROTOC ${PROTOBUF_INSTALL_DIR}/bin/protoc)

custom_protobuf_generate_cpp(GREET_PROTO_SRCS GREET_PROTO_HDRS greet.proto)
add_library(greet_proto STATIC ${GREET_PROTO_SRCS} ${GREET_PROTO_HDRS})
target_include_directories(greet_proto PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(greet_proto PUBLIC protobuf)

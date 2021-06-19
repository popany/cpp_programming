include(grpc.cmake)

# chat proto file
get_filename_component(chat_proto "./proto/chat.proto" ABSOLUTE)
get_filename_component(chat_proto_path "${chat_proto}" PATH)

# generated chat sources
set(chat_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/chat.pb.cc")
set(chat_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/chat.pb.h")
set(chat_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/chat.grpc.pb.cc")
set(chat_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/chat.grpc.pb.h")
add_custom_command(
    OUTPUT "${chat_proto_srcs}" "${chat_proto_hdrs}" "${chat_grpc_srcs}" "${chat_grpc_hdrs}"
    COMMAND ${_PROTOBUF_PROTOC}
    ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${chat_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${chat_proto}"
    DEPENDS "${chat_proto}")

# hello proto file
get_filename_component(hello_proto "./proto/hello.proto" ABSOLUTE)
get_filename_component(hello_proto_path "${hello_proto}" PATH)

# generated hello sources
set(hello_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/hello.pb.cc")
set(hello_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/hello.pb.h")
set(hello_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/hello.grpc.pb.cc")
set(hello_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/hello.grpc.pb.h")
add_custom_command(
    OUTPUT "${hello_proto_srcs}" "${hello_proto_hdrs}" "${hello_grpc_srcs}" "${hello_grpc_hdrs}"
    COMMAND ${_PROTOBUF_PROTOC}
    ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${hello_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${hello_proto}"
    DEPENDS "${hello_proto}")

# goodbye proto file
get_filename_component(goodbye_proto "./proto/goodbye.proto" ABSOLUTE)
get_filename_component(goodbye_proto_path "${goodbye_proto}" PATH)

# generated goodbye sources
set(goodbye_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/goodbye.pb.cc")
set(goodbye_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/goodbye.pb.h")
set(goodbye_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/goodbye.grpc.pb.cc")
set(goodbye_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/goodbye.grpc.pb.h")
add_custom_command(
    OUTPUT "${goodbye_proto_srcs}" "${goodbye_proto_hdrs}" "${goodbye_grpc_srcs}" "${goodbye_grpc_hdrs}"
    COMMAND ${_PROTOBUF_PROTOC}
    ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${goodbye_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${goodbye_proto}"
    DEPENDS "${goodbye_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# grpc_proto
add_library(grpc_proto
    ${chat_grpc_srcs}
    ${chat_grpc_hdrs}
    ${chat_proto_srcs}
    ${chat_proto_hdrs}
    ${hello_grpc_srcs}
    ${hello_grpc_hdrs}
    ${hello_proto_srcs}
    ${hello_proto_hdrs}
    ${goodbye_grpc_srcs}
    ${goodbye_grpc_hdrs}
    ${goodbye_proto_srcs}
    ${goodbye_proto_hdrs})

target_link_libraries(grpc_proto
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF})

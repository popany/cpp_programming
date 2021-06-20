set(grpc_service_srcs
    chat_service.cpp
    goodbye_service.cpp
    hello_service.cpp)

add_library(grpc_service
    ${grpc_service_srcs})

target_link_libraries(grpc_service
    logger
    grpc_proto)

target_include_directories(grpc_service
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})

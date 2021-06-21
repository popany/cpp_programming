set(hello_service_srcs
    hello_service.cpp)

add_library(hello_service
    ${hello_service_srcs})

target_link_libraries(hello_service
    logger
    grpc_protos)

target_include_directories(hello_service
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})

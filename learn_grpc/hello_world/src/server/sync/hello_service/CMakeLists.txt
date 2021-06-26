set(sync_hello_service_srcs
    hello_service.cpp)

add_library(sync_hello_service
    ${sync_hello_service_srcs})

target_link_libraries(sync_hello_service
    logger
    grpc_protos)

target_include_directories(sync_hello_service
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})

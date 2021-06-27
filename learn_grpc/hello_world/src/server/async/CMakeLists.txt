set(async_server_srcs
    server_proactor.cpp
    async_server.cpp)

add_library(async_server
    ${async_server_srcs})

target_link_libraries(async_server
    logger
    utils
    abstract_config
    grpc_protos
    event)

target_include_directories(async_server
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})

set(sync_client_srcs
    chat_client.cpp
    goodbye_client.cpp
    hello_client.cpp)

add_library(sync_client
    ${sync_client_srcs})

target_link_libraries(sync_client
    logger
    utils
    grpc_protos)

target_include_directories(sync_client
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})

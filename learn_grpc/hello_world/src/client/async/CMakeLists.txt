set(async_client_srcs
    client_proactor.cpp
    async_chat_client.cpp
    async_goodbye_client.cpp
    async_hello_client.cpp)

add_library(async_client
    ${async_client_srcs})

target_link_libraries(async_client
    logger
    utils
    abstract_config
    grpc_protos
    event)

target_include_directories(async_client
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})

set(async_client_srcs
    abstract_async_client.cpp
    async_chat_client.cpp
    async_goodbye_client.cpp
    async_hello_client.cpp)

add_library(async_client
    ${async_client_srcs})

target_link_libraries(async_client
    logger
    utils
    grpc_protos)

target_include_directories(async_client
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})

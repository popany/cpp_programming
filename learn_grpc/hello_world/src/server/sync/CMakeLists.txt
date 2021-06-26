add_subdirectory(chat_service)
add_subdirectory(goodbye_service)
add_subdirectory(hello_service)

set(sync_server_srcs
    sync_server.cpp)

add_library(sync_server
    ${sync_server_srcs})

target_link_libraries(sync_server
    logger
    abstract_config
    sync_chat_service
    sync_goodbye_service
    sync_hello_service)

target_include_directories(sync_server
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})

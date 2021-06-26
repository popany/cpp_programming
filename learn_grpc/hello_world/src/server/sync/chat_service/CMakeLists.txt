set(sync_chat_service_srcs
    chat_service.cpp)

add_library(sync_chat_service
    ${sync_chat_service_srcs})

target_link_libraries(sync_chat_service
    logger
    utils
    grpc_protos)

target_include_directories(sync_chat_service
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})

set(chat_service_srcs
    chat_service.cpp)

add_library(chat_service
    ${chat_service_srcs})

target_link_libraries(chat_service
    logger
    utils
    grpc_protos)

target_include_directories(chat_service
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})

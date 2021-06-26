set(sync_goodbye_service_srcs
    goodbye_service.cpp)

add_library(sync_goodbye_service
    ${sync_goodbye_service_srcs})

target_link_libraries(sync_goodbye_service
    logger
    grpc_protos)

target_include_directories(sync_goodbye_service
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})

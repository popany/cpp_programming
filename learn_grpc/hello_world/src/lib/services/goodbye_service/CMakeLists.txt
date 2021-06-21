set(goodbye_service_srcs
    goodbye_service.cpp)

add_library(goodbye_service
    ${goodbye_service_srcs})

target_link_libraries(goodbye_service
    logger
    grpc_protos)

target_include_directories(goodbye_service
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})

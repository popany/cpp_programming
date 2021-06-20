set(client_srcs
    main.cpp)

add_executable(client
    ${client_srcs})

target_link_libraries(client
    logger
    abstract_config
    grpc_proto)

install(TARGETS client
    RUNTIME
    DESTINATION ${PROJECT_SOURCE_DIR}/bin)

install(FILES ${PROJECT_SOURCE_DIR}/config/client.config
    DESTINATION ${PROJECT_SOURCE_DIR}/bin)

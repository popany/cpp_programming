set(server_srcs
    server.cpp
    main.cpp)

add_executable(server
    ${server_srcs})

target_link_libraries(server
    logger
    abstract_config
    chat_service
    goodbye_service
    hello_service)

install(TARGETS server
    RUNTIME
    DESTINATION ${PROJECT_SOURCE_DIR}/bin)

install(FILES ${PROJECT_SOURCE_DIR}/config/server.config
    DESTINATION ${PROJECT_SOURCE_DIR}/bin)

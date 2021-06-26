add_subdirectory(sync)
add_subdirectory(async)

set(server_srcs
    main.cpp)

add_executable(server
    ${server_srcs})

target_link_libraries(server
    logger
    abstract_config
    sync_server
    async_server)

install(TARGETS server
    RUNTIME
    DESTINATION ${PROJECT_SOURCE_DIR}/bin)

install(FILES ${PROJECT_SOURCE_DIR}/config/server.config
    DESTINATION ${PROJECT_SOURCE_DIR}/bin)

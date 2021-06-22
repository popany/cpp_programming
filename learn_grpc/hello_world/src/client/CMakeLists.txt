add_subdirectory(sync)
add_subdirectory(async)

set(client_srcs
    main.cpp)

add_executable(client
    ${client_srcs})

target_link_libraries(client
    logger
    utils
    abstract_config
    sync_client
    async_client)

install(TARGETS client
    RUNTIME
    DESTINATION ${PROJECT_SOURCE_DIR}/bin)

install(FILES ${PROJECT_SOURCE_DIR}/config/client.config
    DESTINATION ${PROJECT_SOURCE_DIR}/bin)

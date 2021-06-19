add_subdirectory(config)

set(client_srcs
    main.cpp)

add_executable(client
    ${client_srcs})

target_link_libraries(client
    logger
    client_config)

install(TARGETS client
    RUNTIME
    DESTINATION ${PROJECT_SOURCE_DIR}/bin)

install(FILES ${PROJECT_SOURCE_DIR}/client.config
    DESTINATION ${PROJECT_SOURCE_DIR}/bin)

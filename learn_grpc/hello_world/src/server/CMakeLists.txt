add_subdirectory(config)

set(server_srcs
    main.cpp)

add_executable(server
    ${server_srcs})

target_link_libraries(server
    logger
    server_config)

install(TARGETS server
    RUNTIME
    DESTINATION ${PROJECT_SOURCE_DIR}/bin)

install(FILES ${PROJECT_SOURCE_DIR}/server.config
    DESTINATION ${PROJECT_SOURCE_DIR}/bin)

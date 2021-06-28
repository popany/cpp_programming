add_subdirectory(sync)
add_subdirectory(async)

set(server_srcs
    main.cpp)

add_executable(server
    ${server_srcs})

set(Boost_USE_STATIC_LIBS ON) # only find static libs
set(Boost_USE_DEBUG_LIBS OFF) # ignore debug libs and 
set(Boost_USE_RELEASE_LIBS ON) # only find release libs 
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.66.0 REQUIRED COMPONENTS date_time)

target_link_libraries(server
    logger
    abstract_config
    sync_server
    async_server
    Boost::date_time)

install(TARGETS server
    RUNTIME
    DESTINATION ${PROJECT_SOURCE_DIR}/bin)

install(FILES ${PROJECT_SOURCE_DIR}/config/server.config
    DESTINATION ${PROJECT_SOURCE_DIR}/bin)

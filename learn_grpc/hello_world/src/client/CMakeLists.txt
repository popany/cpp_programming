add_subdirectory(sync)
add_subdirectory(async)

set(client_srcs
    main.cpp)

add_executable(client
    ${client_srcs})

set(Boost_USE_STATIC_LIBS ON) # only find static libs
set(Boost_USE_DEBUG_LIBS OFF) # ignore debug libs and 
set(Boost_USE_RELEASE_LIBS ON) # only find release libs 
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.66.0 REQUIRED COMPONENTS date_time)

target_link_libraries(client
    logger
    utils
    abstract_config
    sync_client
    async_client
    Boost::date_time)

install(TARGETS client
    RUNTIME
    DESTINATION ${PROJECT_SOURCE_DIR}/bin)

install(FILES ${PROJECT_SOURCE_DIR}/config/client.config
    DESTINATION ${PROJECT_SOURCE_DIR}/bin)

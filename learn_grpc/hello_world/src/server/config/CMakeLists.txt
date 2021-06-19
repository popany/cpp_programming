set(server_config_srcs
    server_config.cpp)

add_library(server_config
    ${server_config_srcs})

target_link_libraries(server_config
    PRIVATE
        logger)

target_include_directories(server_config
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})

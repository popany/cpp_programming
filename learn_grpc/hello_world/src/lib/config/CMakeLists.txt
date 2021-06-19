set(abstract_config_srcs
    abstract_config.cpp)

add_library(abstract_config
    ${abstract_config_srcs})

set(Boost_USE_STATIC_LIBS ON) # only find static libs
set(Boost_USE_DEBUG_LIBS OFF) # ignore debug libs and 
set(Boost_USE_RELEASE_LIBS ON) # only find release libs 
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.70.0)

target_link_libraries(abstract_config
    PRIVATE
        logger)

target_include_directories(abstract_config
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${Boost_INCLUDE_DIRS})

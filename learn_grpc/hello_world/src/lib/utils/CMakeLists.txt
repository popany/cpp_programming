set(utils_srcs
    utils.cpp)

add_library(utils ${utils_srcs})

set(Boost_USE_STATIC_LIBS ON) # only find static libs
set(Boost_USE_DEBUG_LIBS OFF) # ignore debug libs and 
set(Boost_USE_RELEASE_LIBS ON) # only find release libs 
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.66.0 REQUIRED COMPONENTS date_time)

target_link_libraries(utils
    PUBLIC
        Boost::date_time
        logger)

target_include_directories(utils
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${Boost_INCLUDE_DIRS})

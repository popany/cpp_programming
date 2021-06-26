set(proactor_srcs
    proactor.cpp)

add_library(proactor ${proactor_srcs})

target_link_libraries(proactor
    PRIVATE
        logger)

target_include_directories(proactor
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})

cmake_minimum_required(VERSION 3.16.1)

project(demo_1)

add_subdirectory(foo)

install(TARGETS foo
    RUNTIME
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin
    )

install(FILES foo.conf
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin
    )

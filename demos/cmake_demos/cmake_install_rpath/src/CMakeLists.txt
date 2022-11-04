add_executable(demo main.cpp)

target_link_libraries(demo PRIVATE foo)

install(TARGETS demo DESTINATION bin)


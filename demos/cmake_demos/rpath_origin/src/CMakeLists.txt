set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")

add_executable(demo main.cpp)

target_link_libraries(demo PRIVATE foo)

install(TARGETS demo DESTINATION bin)


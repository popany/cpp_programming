set(test_all_srcs
    main.cpp
    test_client.cpp
    test_server.cpp)

add_executable(test_all ${test_all_srcs})

target_link_libraries(test_all
    gtest
    gmock)

add_test(NAME test
    COMMAND test_all)

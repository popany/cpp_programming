set(TEST_CHAT_SRCS
    main.cpp
)

add_executable(test_chat ${TEST_CHAT_SRCS})

target_link_libraries(test_chat
    gtest
    gmock
)

add_test(NAME test_chat
    COMMAND test_chat
)

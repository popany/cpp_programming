set(USE_MYSQL_SRCS
    mysql_accessor.cpp
)

add_executable (use_mysql
    ${USE_MYSQL_SRCS}
)

if(WIN32)
    target_link_libraries(use_mysql
        libmysql
    )

    add_custom_command(TARGET use_mysql POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${LIB_SSL_DLL_PATH}
            $<TARGET_FILE_DIR:use_mysql>
    )

    add_custom_command(TARGET use_mysql POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${LIB_CRYPTO_DLL_PATH}
            $<TARGET_FILE_DIR:use_mysql>
    )
endif()

if (UNIX)
    target_link_libraries(use_mysql
        mysqlcppconn8
    )

    target_include_directories(use_mysql
        PRIVATE
        /usr/include/mysql-cppconn-8
    )

endif()


include(ExternalProject)

ExternalProject_Add(
    mysql_xDevApi
    # https://dev.mysql.com/downloads/connector/cpp/
    URL https://downloads.mysql.com/archives/get/p/20/file/mysql-connector-c%2B%2B-8.0.18-win32.zip
    TIMEOUT 20
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}

    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""

    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
)

add_library(libmysql IMPORTED STATIC GLOBAL)
add_dependencies(libmysql mysql_xDevApi)
set(MYSQL_INCLUDE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/src/mysql_xDevApi/include")
file(MAKE_DIRECTORY ${MYSQL_INCLUDE_DIRECTORY})
target_include_directories(libmysql
    INTERFACE
        ${MYSQL_INCLUDE_DIRECTORY}
)
set(MYSQL_THIRD_LIBS libssl libcrypto)
set_target_properties(libmysql PROPERTIES
    "IMPORTED_LOCATION" "${CMAKE_CURRENT_BINARY_DIR}/src/mysql_xDevApi/lib/vs14/mysqlcppconn8-static.lib"
    "INTERFACE_LINK_LIBRARIES" "${MYSQL_THIRD_LIBS}"
)

add_library(libssl IMPORTED STATIC GLOBAL)
add_dependencies(libssl mysql_xDevApi)
set_target_properties(libssl PROPERTIES
    "IMPORTED_LOCATION" "${CMAKE_CURRENT_BINARY_DIR}/src/mysql_xDevApi/lib/vs14/libssl.lib"
)

add_library(libcrypto IMPORTED STATIC GLOBAL)
add_dependencies(libcrypto mysql_xDevApi)
set_target_properties(libcrypto PROPERTIES
    "IMPORTED_LOCATION" "${CMAKE_CURRENT_BINARY_DIR}/src/mysql_xDevApi/lib/vs14/libcrypto.lib"
)

set(LIB_SSL_DLL_PATH "${CMAKE_CURRENT_BINARY_DIR}/src/mysql_xDevApi/lib/libssl-1_1.dll" PARENT_SCOPE)
set(LIB_CRYPTO_DLL_PATH "${CMAKE_CURRENT_BINARY_DIR}/src/mysql_xDevApi/lib/libcrypto-1_1.dll" PARENT_SCOPE)

#include(CMakePrintHelpers)
#cmake_print_variables(MYSQL_INCLUDE_DIRECTORY)

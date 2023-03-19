include(ExternalProject)
ExternalProject_Add(glog_extern
  URL               https://github.com/google/glog/archive/refs/tags/v0.6.0.tar.gz

  PREFIX            ${CMAKE_CURRENT_BINARY_DIR}/glog_extern
  STAMP_DIR         ${CMAKE_CURRENT_BINARY_DIR}/glog_extern/stamp
  DOWNLOAD_DIR      ${CMAKE_CURRENT_BINARY_DIR}/glog_extern/download
  SOURCE_DIR        ${CMAKE_CURRENT_BINARY_DIR}/glog_extern/source
  BINARY_DIR        ${CMAKE_CURRENT_BINARY_DIR}/glog_extern/binary
  INSTALL_DIR       ${CMAKE_CURRENT_BINARY_DIR}/glog_extern/install
  LOG_DIR           ${CMAKE_CURRENT_BINARY_DIR}/glog_extern/log

  CMAKE_ARGS        -DBUILD_SHARED_LIBS=ON
                    -DWITH_GTEST=OFF
                    -DWITH_GFLAGS=OFF
                    -DBUILD_TESTING=OFF
                    -DCMAKE_BUILD_TYPE=Release
                    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/glog_extern/install

  TEST_COMMAND      ""
)

add_library(glog IMPORTED SHARED GLOBAL)
add_dependencies(glog glog_extern)

ExternalProject_Get_Property(glog_extern INSTALL_DIR)
set(INCLUDE_DIRECTORY "${INSTALL_DIR}/include")
file(MAKE_DIRECTORY ${INCLUDE_DIRECTORY})

target_include_directories(glog INTERFACE ${INCLUDE_DIRECTORY})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(glog INTERFACE Threads::Threads)

set_target_properties(glog PROPERTIES
  "IMPORTED_LOCATION" "${INSTALL_DIR}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}glog${CMAKE_SHARED_LIBRARY_SUFFIX}"
  "INSTALL_DIR" "${INSTALL_DIR}")

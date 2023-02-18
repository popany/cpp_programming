include(ExternalProject)
ExternalProject_Add(google-benchmark
  GIT_REPOSITORY    https://github.com/google/benchmark.git
  GIT_TAG           v1.7.1

  PREFIX            ${CMAKE_CURRENT_BINARY_DIR}/google-benchmark
  TMP_DIR           ${CMAKE_CURRENT_BINARY_DIR}/google-benchmark/tmp
  STAMP_DIR         ${CMAKE_CURRENT_BINARY_DIR}/google-benchmark/stamp
  DOWNLOAD_DIR      ${CMAKE_CURRENT_BINARY_DIR}/google-benchmark/download
  SOURCE_DIR        ${CMAKE_CURRENT_BINARY_DIR}/google-benchmark/source
  BINARY_DIR        ${CMAKE_CURRENT_BINARY_DIR}/google-benchmark/binary
  INSTALL_DIR       ${CMAKE_CURRENT_BINARY_DIR}/google-benchmark/install
  LOG_DIR           ${CMAKE_CURRENT_BINARY_DIR}/google-benchmark/log

  CMAKE_ARGS        -DBENCHMARK_DOWNLOAD_DEPENDENCIES=on
                    -DCMAKE_BUILD_TYPE=Release
                    -DBENCHMARK_ENABLE_TESTING=OFF
                    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/google-benchmark/install
  TEST_COMMAND      ""
  LOG_DOWNLOAD      ON
  LOG_CONFIGURE     ON
  LOG_BUILD         ON
  LOG_INSTALL       ON
)

add_library(benchmark IMPORTED STATIC GLOBAL)
add_dependencies(benchmark google-benchmark)

add_library(benchmark_main IMPORTED STATIC GLOBAL)
add_dependencies(benchmark_main google-benchmark)

ExternalProject_Get_Property(google-benchmark INSTALL_DIR)
set(INCLUDE_DIRECTORY "${INSTALL_DIR}/include")
file(MAKE_DIRECTORY ${INCLUDE_DIRECTORY})

target_include_directories(benchmark INTERFACE ${INCLUDE_DIRECTORY})
target_include_directories(benchmark_main INTERFACE ${INCLUDE_DIRECTORY})

set_target_properties(benchmark PROPERTIES
  "IMPORTED_LOCATION" "${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}benchmark${CMAKE_STATIC_LIBRARY_SUFFIX}"
  "INSTALL_DIR" "${INSTALL_DIR}")

set_target_properties(benchmark_main PROPERTIES
  "IMPORTED_LOCATION" "${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}benchmark_main${CMAKE_STATIC_LIBRARY_SUFFIX}"
  "INSTALL_DIR" "${INSTALL_DIR}")

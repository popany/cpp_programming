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

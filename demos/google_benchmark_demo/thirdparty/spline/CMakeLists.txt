include(ExternalProject)
ExternalProject_Add(tk-spline
  GIT_REPOSITORY    https://github.com/ttk592/spline.git
  GIT_TAG           master

  PREFIX            ${CMAKE_CURRENT_BINARY_DIR}/tk-spline
  STAMP_DIR         ${CMAKE_CURRENT_BINARY_DIR}/tk-spline/stamp
  DOWNLOAD_DIR      ${CMAKE_CURRENT_BINARY_DIR}/tk-spline/download
  SOURCE_DIR        ${CMAKE_CURRENT_BINARY_DIR}/tk-spline/source
  BINARY_DIR        ${CMAKE_CURRENT_BINARY_DIR}/tk-spline/binary
  INSTALL_DIR       ${CMAKE_CURRENT_BINARY_DIR}/tk-spline/install
  LOG_DIR           ${CMAKE_CURRENT_BINARY_DIR}/tk-spline/log

  CONFIGURE_COMMAND ""
  BUILD_COMMAND     ""
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
)

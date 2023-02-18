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

add_library(spline INTERFACE)
add_dependencies(spline tk-spline)

ExternalProject_Get_Property(tk-spline SOURCE_DIR)
set(INCLUDE_DIRECTORY "${SOURCE_DIR}/src")
file(MAKE_DIRECTORY ${INCLUDE_DIRECTORY})

target_include_directories(spline INTERFACE ${INCLUDE_DIRECTORY})

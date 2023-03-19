include(ExternalProject)
ExternalProject_Add(pybind_extern
  URL               https://github.com/pybind/pybind11/archive/refs/tags/v2.10.4.tar.gz

  PREFIX            ${CMAKE_CURRENT_BINARY_DIR}/pybind_extern
  STAMP_DIR         ${CMAKE_CURRENT_BINARY_DIR}/pybind_extern/stamp
  DOWNLOAD_DIR      ${CMAKE_CURRENT_BINARY_DIR}/pybind_extern/download
  SOURCE_DIR        ${CMAKE_CURRENT_BINARY_DIR}/pybind_extern/source
  BINARY_DIR        ${CMAKE_CURRENT_BINARY_DIR}/pybind_extern/binary
  INSTALL_DIR       ${CMAKE_CURRENT_BINARY_DIR}/pybind_extern/install
  LOG_DIR           ${CMAKE_CURRENT_BINARY_DIR}/pybind_extern/log

  CONFIGURE_COMMAND ""
  BUILD_COMMAND     ""
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
)

add_library(pybind11 INTERFACE)
add_dependencies(pybind11 pybind_extern)

ExternalProject_Get_Property(pybind_extern SOURCE_DIR)
set(INCLUDE_DIRECTORY "${SOURCE_DIR}/include")
file(MAKE_DIRECTORY ${INCLUDE_DIRECTORY})

target_include_directories(pybind11 INTERFACE ${INCLUDE_DIRECTORY})

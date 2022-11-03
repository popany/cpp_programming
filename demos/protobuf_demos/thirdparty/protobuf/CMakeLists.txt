include(ExternalProject)
set(PROTOBUF_TAR_GZ https://github.com/protocolbuffers/protobuf/archive/refs/tags/v21.9.tar.gz)

ExternalProject_Add(
  protobuf_external
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/protobuf
  URL ${PROTOBUF_TAR_GZ}
  BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/protobuf
  CMAKE_CACHE_ARGS
    "-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}"
    "-Dprotobuf_BUILD_TESTS:BOOL=OFF"
    "-Dprotobuf_BUILD_EXAMPLES:BOOL=OFF"
    "-Dprotobuf_WITH_ZLIB:BOOL=ON"
    "-DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}"
    "-DBUILD_SHARED_LIBS:BOOL=ON"
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/protobuf"

  SOURCE_SUBDIR cmake
  LOG_DOWNLOAD ON
  LOG_CONFIGURE ON
  LOG_BUILD ON
  LOG_INSTALL ON
  BUILD_ALWAYS 1
  STEP_TARGETS install
)

EXternalProject_Get_Property(protobuf_external install_dir)
add_library(protobuf IMPORTED STATIC GLOBAL)
add_dependencies(protobuf protobuf_external)

set(PROTOBUF_INCLUDE_DIRECTORY "${install_dir}/include")
file(MAKE_DIRECTORY ${PROTOBUF_INCLUDE_DIRECTORY})
target_include_directories(protobuf INTERFACE ${PROTOBUF_INCLUDE_DIRECTORY})

set(PROTOBUF_LIB_FILE_NAME "${CMAKE_SHARED_LIBRARY_PREFIX}protobuf${CMAKE_SHARED_LIBRARY_SUFFIX}")
set_target_properties(protobuf PROPERTIES
  "IMPORTED_LOCATION" "${install_dir}/lib/${PROTOBUF_LIB_FILE_NAME}"
  "INSTALL_DIR" "${install_dir}")


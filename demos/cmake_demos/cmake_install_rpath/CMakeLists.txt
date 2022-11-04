cmake_minimum_required(VERSION 3.24)

project(demo)

set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)

###############################################################################
# https://dev.my-gate.net/2021/08/04/understanding-rpath-with-cmake/

# use, i.e. don't skip the full RPATH for the build tree
set(CMAKE_SKIP_BUILD_RPATH FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
###############################################################################

add_subdirectory(src)
add_subdirectory(lib)


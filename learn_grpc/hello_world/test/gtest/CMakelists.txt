find_package(Threads REQUIRED)

# Enable ExternalProject CMake module
include(ExternalProject)

# Download and install GoogleTest
ExternalProject_Add(
    googletest
    URL https://github.com/google/googletest/archive/release-1.11.0.zip
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/googletest

    CMAKE_ARGS "-Dgtest_force_shared_crt=ON"
    
    # build as release for Unix Makefiles
    CMAKE_BUILD_TYPE "Release"

    # build as release for visual studio 
    BUILD_COMMAND "${CMAKE_COMMAND}" "--build" "${CMAKE_CURRENT_BINARY_DIR}/googletest/src/googletest-build" "--config" "Release"

    # Disable install step
    INSTALL_COMMAND ""

    # Wrap download, configure and build steps in a script to log output
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
)

ExternalProject_Get_Property(googletest source_dir binary_dir)
set(GTEST_HOME "${source_dir}/googletest" CACHE INTERNAL "") # used by mockcpp

if(MSVC)
    Set(GTEST_OUTPUT_DIR "${binary_dir}/lib/Release")
    Set(GMOCK_OUTPUT_DIR "${binary_dir}/lib/Release")
else()
    Set(GTEST_OUTPUT_DIR "${binary_dir}/lib")
    Set(GMOCK_OUTPUT_DIR "${binary_dir}/lib")
endif()

add_library(gtest IMPORTED STATIC GLOBAL)
add_dependencies(gtest googletest)
set(GTEST_INCLUDE_DIRECTORY "${source_dir}/googletest/include")
file(MAKE_DIRECTORY ${GTEST_INCLUDE_DIRECTORY})
target_include_directories(gtest
	INTERFACE
		${GTEST_INCLUDE_DIRECTORY}
)
set(GTEST_LIB_FILE_NAME "${CMAKE_STATIC_LIBRARY_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX}")
set_target_properties(gtest PROPERTIES
	"IMPORTED_LOCATION" "${GTEST_OUTPUT_DIR}/${GTEST_LIB_FILE_NAME}"
	"IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

add_library(gmock IMPORTED STATIC GLOBAL)
add_dependencies(gmock googletest)
set(GMOCK_INCLUDE_DIRECTORY "${source_dir}/googlemock/include")
file(MAKE_DIRECTORY ${GMOCK_INCLUDE_DIRECTORY})
target_include_directories(gmock
	INTERFACE
		${GMOCK_INCLUDE_DIRECTORY}
)
set(GMOCK_LIB_FILE_NAME "${CMAKE_STATIC_LIBRARY_PREFIX}gmock${CMAKE_STATIC_LIBRARY_SUFFIX}")
set_target_properties(gmock PROPERTIES
	"IMPORTED_LOCATION" "${GMOCK_OUTPUT_DIR}/${GMOCK_LIB_FILE_NAME}"
	"IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

add_library(curve_py SHARED)

set_property(TARGET curve_py PROPERTY POSITION_INDEPENDENT_CODE ON)

target_sources(curve_py
  PRIVATE
  curve_py.cc)

find_package(Python REQUIRED Development)
if (Python_FOUND)
  message(STATUS "Python_INCLUDE_DIRS: ${Python_INCLUDE_DIRS}")
  message(STATUS "Python_LIBRARIES: ${Python_LIBRARIES}")
else()
  message(FATAL_ERROR "Failed to find Python3")
endif()

target_include_directories(curve_py
  PRIVATE
  ${Python_INCLUDE_DIRS})

target_link_libraries(curve_py
  PRIVATE
  curve
  glog
  pybind11
  ${Python_LIBRARIES})

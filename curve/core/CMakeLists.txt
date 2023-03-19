add_library(curve SHARED)

target_sources(curve
  PRIVATE
  bezier_curve.cc)

set_property(TARGET curve PROPERTY POSITION_INDEPENDENT_CODE ON)

target_link_libraries(curve
  PRIVATE
  glog)

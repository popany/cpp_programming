add_executable(demo demo.cc)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_link_libraries(demo
  PRIVATE
  benchmark
  benchmark_main
  Threads::Threads
)

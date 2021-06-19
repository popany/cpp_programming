# Hello World

- [Hello World](#hello-world)
  - [Build](#build)
    - [Linux](#linux)
    - [Win32](#win32)

## Build

### Linux

    export GRPC_INSTALL_DIR=/usr1/grpc-debug
    cmake -D CMAKE_PREFIX_PATH=$GRPC_INSTALL_DIR \
        -DCMAKE_BUILD_TYPE=Debug \
        ..

### Win32


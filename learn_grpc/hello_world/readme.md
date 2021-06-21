# Hello World

- [Hello World](#hello-world)
  - [Grpc Version](#grpc-version)
  - [Build](#build)
    - [Linux](#linux)
    - [Win32](#win32)


## Grpc Version

    1.34.0

## Build

### Linux

    mkdir build
    cd build
    export GRPC_INSTALL_DIR=/usr1/grpc-debug
    cmake -D CMAKE_PREFIX_PATH=$GRPC_INSTALL_DIR \
        -DCMAKE_BUILD_TYPE=Debug \
        ..
    make install

### Win32


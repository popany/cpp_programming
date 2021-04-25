# Readme

- [Readme](#readme)
  - [Build](#build)
    - [Build with gRPC and all its dependencies already installed](#build-with-grpc-and-all-its-dependencies-already-installed)
    - [Build with FetchContent](#build-with-fetchcontent)

Copy from [C++ HelloWorld example](https://github.com/grpc/grpc/tree/v1.34.0/examples/cpp/helloworld) in gRPC repository, and follow updates in [gRPC C++ Quick start](https://grpc.io/docs/languages/cpp/quickstart/).

## Build

### Build with gRPC and all its dependencies already installed

    export GRPC_INSTALL_DIR=/usr1/grpc-debug
    export LD_LIBRARY_PATH=$GRPC_INSTALL_DIR/lib:$GRPC_INSTALL_DIR/lib64

    mkdir -p cmake/build
    cd cmake/build

    cmake \
      -DCMAKE_PREFIX_PATH=$GRPC_INSTALL_DIR \
      -DCMAKE_BUILD_TYPE=Debug \
      ../..

    make -j4

### Build with FetchContent

    mkdir -p cmake/build
    cd cmake/build

    cmake \
      -DCMAKE_BUILD_TYPE=Debug \
      -DGRPC_FETCHCONTENT=1 \
      ../..

    make -j4

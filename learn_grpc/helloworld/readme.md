# Readme

- [Readme](#readme)
  - [Build](#build)

Copy from [C++ HelloWorld example](https://github.com/grpc/grpc/tree/v1.34.0/examples/cpp/helloworld) in gRPC repository, and follow updates in [gRPC C++ Quick start](https://grpc.io/docs/languages/cpp/quickstart/).

## Build

    export GRPC_INSTALL_DIR=/usr1/grpc-debug
    export LD_LIBRARY_PATH=$GRPC_INSTALL_DIR/lib:$GRPC_INSTALL_DIR/lib64

    mkdir -p cmake/build
    cd cmake/build

    cmake \
      -DCMAKE_PREFIX_PATH=$GRPC_INSTALL_DIR \
      -DCMAKE_BUILD_TYPE=Debug \
      ../..

    make -j4

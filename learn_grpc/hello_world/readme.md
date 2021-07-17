# Hello World

- [Hello World](#hello-world)
  - [Build](#build)
    - [Linux](#linux)
    - [Win32](#win32)
      - [Build & Install gRPC](#build--install-grpc)
      - [Build & Install absl](#build--install-absl)
      - [Build](#build-1)

## Build

### Linux

Grpc Version: 1.34.0

    mkdir build
    cd build
    export GRPC_INSTALL_DIR=/usr1/grpc-debug
    cmake -D CMAKE_PREFIX_PATH=$GRPC_INSTALL_DIR \
        -DCMAKE_BUILD_TYPE=Debug \
        ..

    export http_proxy="socks5://127.0.0.1:60001"
    export https_proxy="socks5://127.0.0.1:60001"

    make install

Grpc Version: 1.38.1

    mkdir build
    cd build
    export GRPC_INSTALL_DIR=/usr1/grpc-debug
    cmake -D CMAKE_PREFIX_PATH=$GRPC_INSTALL_DIR \
        -DCMAKE_BUILD_TYPE=Debug \
        ..

    export http_proxy="socks5://127.0.0.1:60001"
    export https_proxy="socks5://127.0.0.1:60001"

    make install

    # grpc 1.38.1 need install abseil-cpp
    cd ../../third_party/abseil-cpp/
    mkdir -p cmake/build
    cd cmake/build
    cmake ../..
    make install

### Win32

Grpc Version: 1.38.1

#### Build & Install gRPC

powershell

    git clone -b v1.38.1 https://github.com/grpc/grpc
    cd grpc
    git submodule update --init

    md .build
    cd .build

    pushd
    Launch-VsDevShell.ps1
    popd

    cmake .. -G "Visual Studio 16 2019" -A x64 -T v140 -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=C:/local/grpc

    cmake --build . --config Release -j
    cmake --install .

#### Build & Install absl

    cd ..\third_party\abseil-cpp
    md .build
    cd .build

    cmake .. -G "Visual Studio 16 2019" -A x64 -T v140 -DBUILD_TESTING=OFF -DABSL_USE_GOOGLETEST_HEAD=OFF -DCMAKE_INSTALL_PREFIX=C:/local/grpc
    cmake --build . --config Release -j
    cmake --install .

#### Build

    md .build
    cd .build

    pushd
    Launch-VsDevShell.ps1
    popd

    cmake .. -G "Visual Studio 16 2019" -A x64 -T v140 -DCMAKE_PREFIX_PATH=C:/local/grpc

    set HTTP_PROXY="socks5://127.0.0.1:50001"
    set HTTPS_PROXY="socks5://127.0.0.1:50001"
    cmake --build . --config Debug -j

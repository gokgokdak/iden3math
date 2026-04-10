[![License](https://img.shields.io/badge/license-GPLv3-green.svg)](LICENSE)
[![Docs](https://img.shields.io/badge/docs-README-blue)](https://github.com/gokgokdak/iden3math#readme)
[![GitHub stars](https://img.shields.io/github/stars/gokgokdak/iden3math?style=flat)](https://github.com/gokgokdak/iden3math/stargazers)
[![GitHub issues](https://img.shields.io/github/issues/gokgokdak/iden3math)](https://github.com/gokgokdak/iden3math/issues)
[![CMake Multiple Platforms](https://github.com/gokgokdak/iden3math/actions/workflows/cmake-multi-platform.yml/badge.svg?branch=main)](https://github.com/gokgokdak/iden3math/actions/workflows/cmake-multi-platform.yml)


# iden3math

A simple library in C++ 20 standard that replicates and encapsulates the mathematical computations used in the [iden3](https://github.com/iden3) project.

For now, it is only a minimal implementation for Tornado Cash project, not all the mathematical functions are implemented, but it is designed to be extensible for future needs.

## Features

- C++ and Python interfaces for mathematical computations.
- WebAssembly + npm package output for browser and Node.js usage.
- Easy integration with existing C++ and Python projects.
- Comprehensive unit tests for C++, Python, and JavaScript bindings.

## Requirements

### Toolchain
- Compiler toolchain supports C++ 20
- CMake >= 3.18

### Libraries
- GMP static library with headers
- GoogleTest static library with headers

### Python & pip
- Python >= 3.8
- cibuildwheel >= 3.2.1
- scikit-build-core >= 0.11.6
- pybind11 >= 3.0.1

## Build & Compile

The binary output will be placed in the `product` directory.  

Git clone this repository and its submodules:
```shell
git clone https://github.com/gokgokdak/iden3math.git
cd iden3math
git submodule update --init --recursive
```

### Build - C++ Shared Library

1. Create a build directory and navigate into it:
    ```bash
    mkdir build
    cd build
    ```

2. Run CMake to configure and build the project, add `-DGMP_ENABLE_ASM=ON` if you want to enable assembly optimizations for GMP (binary not supported on all platforms):
    ```bash
    cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_CXX_TEST=ON -DGMP_BUILD_TESTS=ON
    cmake --build . --config Release --target install
    ```

### Build - Python C Extension

1. Create a build directory and navigate into it:
    ```bash
    mkdir build-py
    cd build-py
    ```

2. Run CMake to configure and build the project with Python bindings:
    ```bash
    cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_PY=ON
    cmake --build . --config Release --target install
    ```

### Build - Python Wheel Package

1. Ensure you have the required Python packages:
   ```bash
   pip3 install scikit-build-core pybind11 cibuildwheel
   ```

2. Run the `build_whl.py` script with CMake options:
   ```bash
   python3 build_whl.py -DCMAKE_BUILD_TYPE=Release
   ```

3. A copy of `.pyd` or `.so` file will be placed in `test/py` directory, convenient for Python API testing.

### Build - WebAssembly / npm Package

Activate the Emscripten SDK in your current shell so `emcmake` is available

1. Create a build directory and navigate into it:
    ```bash
    mkdir build-wasm
    cd build-wasm
    ```

2. Run CMake to configure and build the project with WebAssembly target:
    ```bash
    emcmake cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_WASM=ON
    cmake --build . --config Release --target install
    ```

The package entrypoint is:

```bash
product/javascript/npm/dist/index.mjs
```

and the publishable npm metadata is generated at:

```bash
product/javascript/npm/package.json
```

## Running Tests

It is **highly** recommended to run all unit tests before using the library, if any cases failed it means the calculations are not correct and the library should not be used.

- For C++ shared library, make sure CMake option `BUILD_CXX_TEST` is set to `ON`:
    ```bash
    cd product/native/{platform}/bin
    ./iden3math_test
    ```
  
- For Python, navigate to the `tests/py` directory and run the unit tests using Python's unittest framework:
    ```bash
    cd test/py
    python3 -m unittest
    ```

- For JavaScript / wasm, build the wasm target and run:
    ```bash
    cd test/js
    node --test *.test.mjs
    ```
  
- To test the Python wheel, remove `.pyd` or `.so` file from `tests/py` directory and install the wheel:
    ```bash
    rm tests/py/*.pyd # Windows
    rm tests/py/*.so  # Linux or MacOS
    pip3 install product/wheel/{platform}.whl
    cd tests/py
    python3 -m unittest
    ```

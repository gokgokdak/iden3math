# iden3math

A simple library in C++ 20 standard that replicates and encapsulates the mathematical computations used in the [iden3](https://github.com/iden3) project.

For now, it is only a minimal implementation for Tornado Cash project, not all the mathematical functions are implemented, but it is designed to be extensible for future needs.

## Features

- C++ and Python interfaces for mathematical computations.
- Easy integration with existing C++ and Python projects.
- Comprehensive unit tests for both C++ and Python.

## Requirements

### Toolchain
- Compiler toolchain supports C++ 20
- CMake >= 3.18

### Libraries
- GMP static library with headers
- GoogleTest static library with headers

### Python & pip
- Python >= 3.8
- cibuildwheel
- scikit-build-core
- pybind11
- unittest

## Build C++ Library

To build the C++ shared object library, follow these steps:

1. Clone the repository and its submodules:
    ```shell
    git clone https://github.com/gokgokdak/iden3math.git
    cd iden3math
    git submodule update --init --recursive
    ```

2. Create a build directory and navigate into it:
    ```bash
    mkdir build
    cd build
    ```

3. Run CMake to configure the project:
    ```bash
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DBUILD_PY=OFF \
        -DBUILD_TEST=ON \
        -DGMP_INC_DIR="{gmp_include_dir}" \
        -DGMP_LIB_DIR="{gmp_library_dir}" \
        -DGTEST_INC_DIR="{googletest_include_dir}" \
        -DGTEST_LIB_DIR="{googletest_library_dir}"
    ```

4. Build the project:
   ```bash
   cmake --build . --config Release --target install
   ```

5. The binary output will be placed in the `product` directory,

## Build Python Wheel

To build the Python wheel, use the `build_whl.py` script, it accepts CMake arguments from command-line and will generate a `pyproject.toml` file and build the wheel using `cibuildwheel`.

1. Ensure you have the required Python packages:
   ```bash
   pip3 install scikit-build-core pybind11 cibuildwheel
   ```

2. Run the `build_whl.py` script with the necessary CMake options:
   ```bash
   python3 build_whl.py -DCMAKE_BUILD_TYPE=Release \
                        -DBUILD_PY=ON \
                        -DBUILD_TEST=ON \
                        -DGMP_INC_DIR="{gmp_include_dir}" \
                        -DGMP_LIB_DIR="{gmp_library_dir}" \
                        -DGTEST_INC_DIR="{googletest_include_dir}" \
                        -DGTEST_LIB_DIR="{googletest_library_dir}"
   ```

3. The wheel file will be generated in the `wheelhouse` directory.

4. A copy of `.pyd` or `.so` file will be placed in `test/py` directory, convenient for Python API testing.

## Running Tests

Make sure CMake option `BUILD_TEST` is set to `ON`

It is **highly** recommended to run all unit tests before using the library, if any cases failed it means the calculations are not correct and the library should not be used.

- For C++, navigate to the `product` directory and run the test executable:
    ```bash
    cd product
    ./iden3math_test
    ```
  
- For Python, navigate to the `tests/py` directory and run the unit tests using Python's unittest framework:
    ```bash
    cd tests/py
    python3 -m unittest
    ```
  
- To test the Python wheel, remove `.pyd` or `.so` file from `tests/py` directory and install the wheel:
    ```bash
    rm tests/py/*.pyd # Windows
    rm tests/py/*.so  # Linux or MacOS
    pip3 install wheelhouse/{choose_whl_for_your_platform}.whl
    cd tests/py
    python3 -m unittest
    ```

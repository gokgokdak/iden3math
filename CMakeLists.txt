cmake_minimum_required(VERSION 3.18)

project(iden3math)

set(CMAKE_CXX_STANDARD 20)
set(PRODUCT_DIR "${CMAKE_SOURCE_DIR}/product")

# Python binding
if(BUILD_PY)
    # Include local Python headers
    find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
    message("-- Python Include: ${Python_INCLUDE_DIRS}")
    message("-- Python Executable: ${Python_EXECUTABLE}")
    include_directories(${Python_INCLUDE_DIRS})

    # Include pybind11
    include_directories(${CMAKE_SOURCE_DIR}/deps/pybind11/include)
    add_subdirectory(${CMAKE_SOURCE_DIR}/deps/pybind11)

    # Get dynamic library suffix
    execute_process(
        COMMAND ${Python_EXECUTABLE} -c "import sysconfig; print(sysconfig.get_config_var('EXT_SUFFIX'))"
        OUTPUT_VARIABLE PYTHON_EXTENSION_SUFFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()

# Dependency GMP
if(NOT GMP_INC_DIR)
    message(FATAL_ERROR "GMP include path not set")
endif()
if(NOT GMP_LIB_DIR)
    message(FATAL_ERROR "GMP library path not set")
endif()
include_directories(${GMP_INC_DIR})
if(WIN32)
    file(GLOB_RECURSE GMP_LIBS "${GMP_LIB_DIR}/*.lib")
else()
    file(GLOB_RECURSE GMP_LIBS "${GMP_LIB_DIR}/*.a")
endif()

# Source files
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)
file(GLOB_RECURSE LIB_SOURCES_CXX "${CMAKE_SOURCE_DIR}/src/cxx/*.cxx")
file(GLOB_RECURSE LIB_SOURCES_PYBIND "${CMAKE_SOURCE_DIR}/src/pyc/*.cxx")

# Shared library
if(BUILD_PY)
    pybind11_add_module(iden3math ${LIB_SOURCES_CXX} ${LIB_SOURCES_PYBIND})
    target_compile_definitions(iden3math PRIVATE IDEN3MATH_BUILD_PY)
    set_target_properties(iden3math PROPERTIES
        OUTPUT_NAME "iden3math"
        PREFIX ""  # No "lib" prefix for Python modules
        SUFFIX "${PYTHON_EXTENSION_SUFFIX}"
    )
else()
    add_library(iden3math SHARED ${LIB_SOURCES_CXX})
    target_compile_definitions(iden3math PRIVATE IDEN3MATH_BUILD_DYNAMIC)
endif()
target_link_libraries(iden3math PRIVATE ${GMP_LIBS})

# Unit tests
if(BUILD_TEST)
    # Dependency gtest
    if(NOT GTEST_INC_DIR)
        message(FATAL_ERROR "GTest include path not set")
    endif()
    if(NOT GTEST_LIB_DIR)
        message(FATAL_ERROR "GTest library path not set")
    endif()
    include_directories(${GTEST_INC_DIR})
    if(WIN32)
        file(GLOB_RECURSE GTEST_LIBS "${GTEST_LIB_DIR}/*.lib")
    else()
        file(GLOB_RECURSE GTEST_LIBS "${GTEST_LIB_DIR}/*.a")
    endif()

    # Executable
    file(GLOB_RECURSE TEST_SOURCES "${CMAKE_SOURCE_DIR}/test/cxx/*.cxx")
    add_executable(iden3math_test ${TEST_SOURCES} ${LIB_SOURCES_CXX})
    target_compile_definitions(iden3math_test PRIVATE IDEN3MATH_BUILD_TEST)
    target_link_libraries(iden3math_test ${GTEST_LIBS} ${GMP_LIBS})

    # Add tests
    enable_testing()
    add_test(NAME iden3math_test COMMAND iden3math_test)
endif()

# Install
set(CMAKE_INSTALL_PREFIX ${PRODUCT_DIR} CACHE PATH "Installation Directory" FORCE)
install(CODE "
    if(EXISTS ${PRODUCT_DIR})
        file(REMOVE_RECURSE ${PRODUCT_DIR})
    endif()
")
if(BUILD_PY)
    install(DIRECTORY src/pyi/ DESTINATION iden3math)
    install(TARGETS iden3math
            LIBRARY DESTINATION iden3math
            ARCHIVE DESTINATION lib
    )
else()
    install(DIRECTORY include/ DESTINATION include)
    install(TARGETS iden3math
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
    )
    if(BUILD_TEST)
        install(TARGETS iden3math_test
                RUNTIME DESTINATION bin
        )
    endif()
endif()

# Make a copy for Python tests
if(BUILD_PY)
    # Delete existing
    install(CODE "
        file(GLOB PYD_FILES \"${CMAKE_SOURCE_DIR}/test/py/*.pyd\")
        file(GLOB SO_FILES  \"${CMAKE_SOURCE_DIR}/test/py/*.so\")
        set(FILES_TO_REMOVE \${PYD_FILES} \${SO_FILES})
        foreach(f \${FILES_TO_REMOVE})
            file(REMOVE \"\${f}\")
        endforeach()
    ")
    # Copy
    install(TARGETS iden3math
            LIBRARY DESTINATION "${CMAKE_SOURCE_DIR}/test/py"
    )
endif()

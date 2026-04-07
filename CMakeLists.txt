cmake_minimum_required(VERSION 3.18)

project(iden3math)

set(CMAKE_CXX_STANDARD 20)
set(IDEN3MATH_TARGET iden3math)
set(IDEN3MATH_TEST_TARGET iden3math_test)
set(PRODUCT_DIR "${CMAKE_SOURCE_DIR}/product")
file(STRINGS "${CMAKE_SOURCE_DIR}/version.txt" IDEN3MATH_VERSION LIMIT_COUNT 1)

# Options
option(BUILD_CXX_TEST "Build unit tests" OFF)
option(BUILD_PY "Build Python bindings" OFF)
option(BUILD_WASM "Build WebAssembly bindings and npm package" OFF)
option(GMP_ENABLE_ASM "Enable GMP assembly optimizations" OFF)
option(GMP_ENABLE_CXX "Enable GMP C++ interface" ON)
option(GMP_BUILD_STATIC "Build static GMP library" ON)
option(GMP_BUILD_SHARED "Build shared GMP library" OFF)
option(GMP_BUILD_TESTS "Build GMP tests" OFF)
if(BUILD_PY AND BUILD_WASM)
    message(FATAL_ERROR "BUILD_PY and BUILD_WASM cannot be enabled at the same time.")
endif()
if(BUILD_WASM AND NOT EMSCRIPTEN)
    message(FATAL_ERROR "BUILD_WASM requires the Emscripten toolchain. Use emcmake cmake ...")
endif()

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
# WebAssembly
elseif(BUILD_WASM)
    set(GMP_ENABLE_ASM OFF CACHE BOOL "" FORCE)
    set(GMP_BUILD_SHARED OFF CACHE BOOL "" FORCE)
    set(GMP_BUILD_STATIC ON CACHE BOOL "" FORCE)
    set(GMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
endif()

# Dependency GMP
add_subdirectory(${CMAKE_SOURCE_DIR}/deps/libgmp-cmake EXCLUDE_FROM_ALL)

# Source files
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)
file(GLOB_RECURSE LIB_SOURCES_CXX "${CMAKE_SOURCE_DIR}/src/cxx/*.cxx")
file(GLOB_RECURSE LIB_SOURCES_PYBIND "${CMAKE_SOURCE_DIR}/src/pyc/*.cxx")
file(GLOB_RECURSE LIB_SOURCES_WASM "${CMAKE_SOURCE_DIR}/src/wasm/*.cxx")

# Target
if(BUILD_PY)
    pybind11_add_module(${IDEN3MATH_TARGET} ${LIB_SOURCES_CXX} ${LIB_SOURCES_PYBIND})
    target_compile_definitions(${IDEN3MATH_TARGET} PRIVATE IDEN3MATH_BUILD_PY)
    set_target_properties(${IDEN3MATH_TARGET} PROPERTIES
        OUTPUT_NAME "iden3math"
        PREFIX ""  # No "lib" prefix for Python modules
        SUFFIX "${PYTHON_EXTENSION_SUFFIX}"
    )
elseif(BUILD_WASM)
    set(IDEN3MATH_TARGET iden3math_raw)
    set(NPM_BUILD_DIR "${CMAKE_BINARY_DIR}/npm")
    set(NPM_DIST_DIR "${NPM_BUILD_DIR}/dist")
    file(MAKE_DIRECTORY "${NPM_DIST_DIR}")

    add_executable(${IDEN3MATH_TARGET} ${LIB_SOURCES_CXX} ${LIB_SOURCES_WASM})
    target_compile_definitions(${IDEN3MATH_TARGET} PRIVATE IDEN3MATH_BUILD_WASM)
    target_compile_options(${IDEN3MATH_TARGET} PRIVATE "SHELL:-fexceptions")
    target_link_options(${IDEN3MATH_TARGET} PRIVATE
        "SHELL:--bind"
        "SHELL:-fexceptions"
        "SHELL:-sALLOW_MEMORY_GROWTH=1"
        "SHELL:-sDISABLE_EXCEPTION_CATCHING=0"
        "SHELL:-sENVIRONMENT=web,worker,node"
        "SHELL:-sEXPORT_ES6=1"
        "SHELL:-sEXPORT_NAME=createIden3MathRawModule"
        "SHELL:-sFILESYSTEM=0"
        "SHELL:-sMODULARIZE=1"
        "SHELL:-sNO_EXIT_RUNTIME=1"
    )
    set_target_properties(${IDEN3MATH_TARGET} PROPERTIES
        OUTPUT_NAME "iden3math_raw"
        PREFIX ""
        SUFFIX ".mjs"
        RUNTIME_OUTPUT_DIRECTORY "${NPM_DIST_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${NPM_DIST_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY "${NPM_DIST_DIR}"
    )
    configure_file("${CMAKE_SOURCE_DIR}/src/js/index.mjs" "${NPM_DIST_DIR}/index.mjs" COPYONLY)
    configure_file("${CMAKE_SOURCE_DIR}/src/js/index.d.ts" "${NPM_DIST_DIR}/index.d.ts" COPYONLY)
    configure_file("${CMAKE_SOURCE_DIR}/src/js/package.json.in" "${NPM_BUILD_DIR}/package.json" @ONLY)
    configure_file("${CMAKE_SOURCE_DIR}/README.md" "${NPM_BUILD_DIR}/README.md" COPYONLY)
    configure_file("${CMAKE_SOURCE_DIR}/LICENSE" "${NPM_BUILD_DIR}/LICENSE" COPYONLY)
else()
    add_library(${IDEN3MATH_TARGET} SHARED ${LIB_SOURCES_CXX})
    target_compile_definitions(${IDEN3MATH_TARGET} PRIVATE IDEN3MATH_BUILD_DYNAMIC)
endif()

# Link
target_link_libraries(${IDEN3MATH_TARGET} PRIVATE GMP::gmpxx)
if(WIN32 AND MINGW)
    target_link_libraries(${IDEN3MATH_TARGET} PRIVATE bcrypt)
endif()

# CXX unit tests
if(BUILD_CXX_TEST)
    # Dependency gtest
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    add_subdirectory(${CMAKE_SOURCE_DIR}/deps/googletest EXCLUDE_FROM_ALL)

    # Executable
    file(GLOB_RECURSE TEST_SOURCES "${CMAKE_SOURCE_DIR}/test/cxx/*.cxx")
    add_executable(${IDEN3MATH_TEST_TARGET} ${TEST_SOURCES} ${LIB_SOURCES_CXX})
    target_compile_definitions(${IDEN3MATH_TEST_TARGET} PRIVATE IDEN3MATH_BUILD_TEST)
    target_link_libraries(${IDEN3MATH_TEST_TARGET} PRIVATE GMP::gmpxx gtest gtest_main)
    if(WIN32 AND MINGW)
        target_link_libraries(${IDEN3MATH_TEST_TARGET} PRIVATE bcrypt)
    endif()

    # Add tests
    enable_testing()
    add_test(NAME ${IDEN3MATH_TEST_TARGET} COMMAND ${IDEN3MATH_TEST_TARGET})
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
elseif(BUILD_WASM)
    install(DIRECTORY "${NPM_BUILD_DIR}/" DESTINATION npm)
else()
    install(DIRECTORY include/ DESTINATION include)
    install(TARGETS iden3math
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
    )
    if(BUILD_CXX_TEST)
        install(TARGETS ${IDEN3MATH_TEST_TARGET}
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

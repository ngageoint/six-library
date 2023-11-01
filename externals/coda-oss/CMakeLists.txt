# - CODA-OSS CMake Build script.
#
# Author:  Scott A. Colcord

cmake_minimum_required(VERSION 3.14)
project(coda-oss)

project(coda-oss)

set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED true)

if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    # build and package with conan
    include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    conan_basic_setup()

    include("${CMAKE_BINARY_DIR}/conan_paths.cmake")
endif()

if (${CMAKE_PROJECT_NAME} STREQUAL coda-oss)
    # this is the top level project

    # Always turn on "warnings as errors." To avoid lots of (meaningless?) build output,
    # we'll dial-back warnings as necessary.
    if (MSVC)
        # set warning level to /W3
        string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        string(REGEX REPLACE "/W[0-4]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

       if (ENABLE_ASAN)
            # https://docs.microsoft.com/en-us/cpp/sanitizers/asan?view=msvc-160
            add_compile_options(/fsanitize=address)
       endif()
    elseif (UNIX)
       add_compile_options(-Werror) # warnings as errors
    endif()

    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
    include(CodaBuild)

    coda_initialize_build()

    # install our cmake build modules for downstream use
    install(DIRECTORY "cmake/"
            DESTINATION "${CODA_STD_PROJECT_LIB_DIR}/cmake/"
            FILES_MATCHING PATTERN "*.cmake")
endif()

add_subdirectory("modules")


cmake_minimum_required(VERSION 3.14)
project(six-library)

set(CMAKE_CXX_STANDARD 20)

if (${CMAKE_PROJECT_NAME} STREQUAL six-library)
    # we are the top-level project and are responsible for configuration

    # set up warnings
    if (MSVC)
	add_compile_options(/std:c++20)
        # set warning level to /W3
        string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        string(REGEX REPLACE "/W[0-4]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
        add_compile_options(/W3 /wd4290 /wd4512)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
    elseif (UNIX)
	add_compile_options(-std=c++20)
        add_compile_options(
            -Wno-deprecated
            -Wno-unused-value
            -Wno-unused-but-set-variable
        )
    endif()

    if (EXISTS "${CMAKE_BINARY_DIR}/conan_paths.cmake")
        # use pre-built dependencies from conan packages
        include("${CMAKE_BINARY_DIR}/conan_paths.cmake")
        find_package(coda-oss REQUIRED)
        find_package(nitro REQUIRED)
        include(CodaBuild)
        coda_initialize_build()
    else()
        # build dependencies ourselves
        set(CODA_OSS_DIR "coda-oss")
        list(APPEND CMAKE_MODULE_PATH
             "${CMAKE_CURRENT_SOURCE_DIR}/externals/${CODA_OSS_DIR}/cmake")
        include(CodaBuild)
        coda_initialize_build()
        add_subdirectory("externals")
    endif()
endif()

# needed to run some unit tests from the build directory
file(COPY "croppedNitfs"
     DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

add_subdirectory(six)

coda_generate_package_config()

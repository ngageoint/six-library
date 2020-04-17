cmake_minimum_required(VERSION 3.14)
project(six-library)

set(CMAKE_CXX_STANDARD 11)

if (${CMAKE_PROJECT_NAME} STREQUAL six-library)
    if (EXISTS ${CMAKE_BINARY_DIR}/conan_paths.cmake)
        # use pre-built coda-oss and nitro from conan packages
        include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
        find_package(coda-oss REQUIRED)
        find_package(nitro REQUIRED)
        include(CodaBuild)
        coda_initialize_build()
    else()
        # build coda-oss and nitro ourselves
        include("externals/coda-oss/cmake/CodaBuild.cmake")
        coda_initialize_build()
        add_subdirectory("externals")
    endif()
else()
    # don't initialize or add externals, the top-level project is responsible for that
endif()

add_subdirectory(six)

coda_generate_package_config()

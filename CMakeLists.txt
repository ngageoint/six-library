cmake_minimum_required(VERSION 3.14)
project(six-library)

set(CMAKE_CXX_STANDARD 14)
set(CXX_STANDARD_REQUIRED true)

if (${CMAKE_PROJECT_NAME} STREQUAL six-library)
    # we are the top-level project and are responsible for configuration

    # Always turn on "warnings as errors" to avoid lots of (meaningless?) build output;
    # we'll dial-back warnings as necessary.
    if (MSVC)
       add_compile_options(/WX) # warnings as errors
    elseif (UNIX)
       add_compile_options(-Werror) # warnings as errors
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

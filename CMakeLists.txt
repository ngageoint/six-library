cmake_minimum_required(VERSION 3.14)
project(nitro)

set(CMAKE_CXX_STANDARD 14)
set(CXX_STANDARD_REQUIRED true)

if (${CMAKE_PROJECT_NAME} STREQUAL nitro)
    # we are the top-level project and are responsible for configuration

    # Always turn on "warnings as errors" to avoid lots of (meaningless?) build output;
    # we'll dial-back warnings as necessary.
    if (MSVC)
       add_compile_options(/WX) # warnings as errors
       add_compile_options(/MP) # multi-processor compile

       if (ENABLE_ASAN)
       	  # https://docs.microsoft.com/en-us/cpp/sanitizers/asan?view=msvc-160
	  add_compile_options(/fsanitize=address)
       endif()

    elseif (UNIX)
       add_compile_options(-Werror) # warnings as errors

       if (ENABLE_ASAN)
       	  # https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html
          add_compile_options(-fsanitize=address)
          add_link_options(-fsanitize=address)
       endif()
    endif()

    if (EXISTS "${CMAKE_BINARY_DIR}/conan_paths.cmake")
        # use pre-built coda-oss from conan package
        include("${CMAKE_BINARY_DIR}/conan_paths.cmake")
        find_package(coda-oss REQUIRED)
        include(CodaBuild)
        coda_initialize_build()
    else()
        # build coda-oss ourselves

        # disable some unneeded drivers from coda-oss
        set(ENABLE_PCRE OFF CACHE BOOL "enable PCRE library" FORCE)
        set(ENABLE_UUID OFF CACHE BOOL "enable uuid library" FORCE)
        set(ENABLE_ZIP OFF CACHE BOOL "enable ZIP library" FORCE)

        set(CODA_OSS_DIR "coda-oss")
        list(APPEND CMAKE_MODULE_PATH
             "${CMAKE_CURRENT_SOURCE_DIR}/externals/${CODA_OSS_DIR}/cmake")
        include(CodaBuild)
        coda_initialize_build()
        add_subdirectory("externals/${CODA_OSS_DIR}")
    endif()
endif()

set(NITRO_C_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/modules/c)
add_subdirectory("modules")

coda_generate_package_config()

cmake_minimum_required(VERSION 3.14)
project(nitro)

set(CMAKE_C_STANDARD 90)
set(CMAKE_CXX_STANDARD 11)

if (${CMAKE_PROJECT_NAME} STREQUAL nitro)
    # we are the top-level project and are responsible for configuration

    # set up warnings
    if (MSVC)
        # set warning level to /W3
        string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        string(REGEX REPLACE "/W[0-4]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
        add_compile_options(/W3 /wd4290 /wd4512)
        add_compile_options(/wd4100) # '...': unreferenced formal parameter
        add_compile_options(/wd4127) # conditional expression is constant
        if (ENABLE_ASAN)
            # https://docs.microsoft.com/en-us/cpp/sanitizers/asan?view=msvc-160
            add_compile_options(/fsanitize=address)
        endif()
        add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
    elseif (UNIX)
        add_compile_options(
            -Wall
            -Wno-deprecated
            -Wno-unused-value
            -Wno-unused-but-set-variable
        )
        if (ENABLE_ASAN)
            # https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html
            add_compile_options(-fsanitize=address)
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

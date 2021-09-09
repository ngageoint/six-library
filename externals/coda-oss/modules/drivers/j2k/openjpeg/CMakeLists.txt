set(TARGET_NAME openjpeg)

if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    # use this CMakeLists.txt to build and package openjpeg with conan
    project(coda-oss_openjpeg)
    include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    conan_basic_setup()
    set(CODA_SKIP_SYSTEM_DEPENDENCIES ON)
    include(CodaBuild.cmake)
    coda_initialize_build()
    coda_generate_package_config()
endif()

if (J2K_HOME)
    find_library(${TARGET_NAME}_PATH
                 ${TARGET_NAME}
                 PATHS ${J2K_HOME}
                 PATH_SUFFIXES lib lib64
                 NO_DEFAULT_PATH)
    if ("${${TARGET_NAME}_PATH}" STREQUAL "${TARGET_NAME}_PATH-NOTFOUND")
        message(FATAL_ERROR "openjpeg not found at J2K_HOME ${J2K_HOME}")
    else()
        message("Found openjpeg at ${${TARGET_NAME}_PATH}")
    endif()

    # verify we can link the found library
    string(REGEX MATCH ".*\.(a|lib)$" OPENJPEG_IS_STATIC ${${TARGET_NAME}_PATH})
    if (OPENJPEG_IS_STATIC)
        set(CMAKE_REQUIRED_DEFINITIONS -DOPJ_STATIC)
    endif()
    set(CMAKE_REQUIRED_INCLUDES "${J2K_HOME}/include")
    set(CMAKE_REQUIRED_LIBRARIES ${${TARGET_NAME}_PATH})
    unset(J2K_HOME_VALID CACHE)
    check_cxx_source_compiles("
        #include <openjpeg.h>
        int main() { auto f = &opj_start_compress; return 0; }"
        J2K_HOME_VALID)
    unset(CMAKE_REQUIRED_LIBRARIES)
    unset(CMAKE_REQUIRED_INCLUDES)
    unset(CMAKE_REQUIRED_DEFINITIONS)
    if (NOT J2K_HOME_VALID)
        message(FATAL_ERROR "Could not link with openjpeg from J2K_HOME ${J2K_HOME}")
    endif()

    # create interface library pointing to the external library
    add_library(${TARGET_NAME} INTERFACE)
    target_include_directories(${TARGET_NAME} INTERFACE "${J2K_HOME}/include")
    target_link_libraries(${TARGET_NAME} INTERFACE ${${TARGET_NAME}_PATH})
    if (OPENJPEG_IS_STATIC)
        target_compile_definitions(${TARGET_NAME} INTERFACE OPJ_STATIC)
    endif()

else()
    coda_fetch_driver(
        NAME ${TARGET_NAME}
        ARCHIVE "openjpeg-2.3.1_mod.tar"
        HASH "SHA256=d25ed5ecbed03e3e1a058317d067dedb6d596277284072912ee434d25d1f58a6"
    )

    set(SOURCE_DIR "${${CMAKE_PROJECT_NAME}_${TARGET_NAME}_SOURCE_DIR}")

    check_include_file("stdint.h" OPJ_HAVE_STDINT_H)
    set(OPENJPEG_VERSION_MAJOR 2)
    set(OPENJPEG_VERSION_MINOR 3)
    set(OPENJPEG_VERSION_BUILD 1)

    check_include_file("inttypes.h" OPJ_HAVE_INTTYPES_H)
    set(PACKAGE_VERSION 2.3.1)
    set(_FILE_OFFSET_BITS 64)
    set(_LARGEFILE_SOURCE 1)
    set(_LARGE_FILES 1)
    set(CMAKE_REQUIRED_DEFINITIONS -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE)
    check_symbol_exists("fseeko" "stdio.h" OPJ_HAVE_FSEEKO)
    unset(CMAKE_REQUIRED_DEFINITIONS)
    check_include_file("malloc.h" OPJ_HAVE_MALLOC_H)
    check_symbol_exists("aligned_alloc" "malloc.h" OPJ_HAVE_ALIGNED_ALLOC)
    check_symbol_exists("_aligned_malloc" "malloc.h" OPJ_HAVE__ALIGNED_MALLOC)
    check_symbol_exists("memalign" "malloc.h" OPJ_HAVE_MEMALIGN)
    check_symbol_exists("posix_memalign" "stdlib.h" OPJ_HAVE_POSIX_MEMALIGN)
    test_big_endian(OPJ_BIG_ENDIAN)

    set(SOURCE_DIR "${${CMAKE_PROJECT_NAME}_${TARGET_NAME}_SOURCE_DIR}")
    configure_file("${SOURCE_DIR}/src/lib/openjp2/opj_config.h.cmake.in"
                   "opj_config.h")
    configure_file("${SOURCE_DIR}/src/lib/openjp2/opj_config_private.h.cmake.in"
                   "opj_config_private.h")

    foreach(src "bio.c" "cio.c" "dwt.c" "event.c" "image.c" "invert.c"
                "j2k.c" "jp2.c" "mct.c" "mqc.c" "openjpeg.c"
                "opj_clock.c" "opj_malloc.c" "pi.c" "sparse_array.c"
                "t1.c" "t2.c" "tcd.c" "thread.c" "tgt.c"
                "function_list.c")
        list(APPEND SOURCES "${SOURCE_DIR}/src/lib/openjp2/${src}")
    endforeach()

    add_library(${TARGET_NAME} ${SOURCES})
    if (NOT BUILD_SHARED_LIBS)
        target_compile_definitions(${TARGET_NAME} PUBLIC OPJ_STATIC)
    endif()
    target_include_directories(${TARGET_NAME}
        PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
               "$<BUILD_INTERFACE:${SOURCE_DIR}/src/lib/openjp2>"
               "$<INSTALL_INTERFACE:${CODA_STD_PROJECT_INCLUDE_DIR}>")
    install(FILES "${SOURCE_DIR}/src/lib/openjp2/openjpeg.h"
                  "${SOURCE_DIR}/src/lib/openjp2/opj_stdint.h"
                  "${CMAKE_CURRENT_BINARY_DIR}/opj_config.h"
                  "${CMAKE_CURRENT_BINARY_DIR}/opj_config_private.h"
            DESTINATION "${CODA_STD_PROJECT_INCLUDE_DIR}"
            ${CODA_INSTALL_OPTION})
endif()
install(TARGETS ${TARGET_NAME}
        EXPORT ${CODA_EXPORT_SET_NAME}
        ${CODA_INSTALL_OPTION}
        LIBRARY DESTINATION "${CODA_STD_PROJECT_LIB_DIR}"
        ARCHIVE DESTINATION "${CODA_STD_PROJECT_LIB_DIR}")

set(J2K_LIB ${TARGET_NAME} CACHE INTERNAL "library providing J2K support")

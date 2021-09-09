set(TARGET_NAME z)

if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    # use this CMakeLists.txt to build and package zlib with conan
    project(coda-oss_zlib)
    include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    conan_basic_setup()
    set(CODA_SKIP_SYSTEM_DEPENDENCIES ON)
    include(CodaBuild.cmake)
    coda_initialize_build()
    set(ENABLE_ZIP ON)
    coda_generate_package_config()
endif()

if (ZIP_HOME)
    find_library(${TARGET_NAME}_PATH
                 ${TARGET_NAME}
                 PATHS ${ZIP_HOME}
                 PATH_SUFFIXES lib lib64
                 NO_DEFAULT_PATH)
    if ("${${TARGET_NAME}_PATH}" STREQUAL "${TARGET_NAME}_PATH-NOTFOUND")
        message(FATAL_ERROR "zlib not found at ZIP_HOME ${ZIP_HOME}")
    else()
        message("Found zlib at ${${TARGET_NAME}_PATH}")
    endif()

    # verify we can link zlib
    set(CMAKE_REQUIRED_INCLUDES "${ZIP_HOME}/include")
    set(CMAKE_REQUIRED_LIBRARIES ${${TARGET_NAME}_PATH})
    unset(ZLIB_HOME_VALID CACHE)
    check_cxx_source_compiles("
        #include <zlib.h>
        int main() { auto f = &inflate; return 0; }"
        ZLIB_HOME_VALID)
    unset(CMAKE_REQUIRED_LIBRARIES)
    unset(CMAKE_REQUIRED_INCLUDES)
    if (NOT ZLIB_HOME_VALID)
        message(FATAL_ERROR "Could not link with zlib from ZIP_HOME ${ZIP_HOME}")
    endif()

    # create interface library pointing to the external zlib
    add_library(${TARGET_NAME} INTERFACE)
    target_include_directories(${TARGET_NAME} INTERFACE "${ZIP_HOME}/include")
    target_link_libraries(${TARGET_NAME} INTERFACE ${${TARGET_NAME}_PATH})


    find_library(minizip_PATH
                 minizip
                 PATHS ${ZIP_HOME}
                 PATH_SUFFIXES lib lib64
                 NO_DEFAULT_PATH)
    if ("${minizip_PATH}" STREQUAL "minizip_PATH-NOTFOUND")
        message(FATAL_ERROR "minizip not found at ZIP_HOME ${ZIP_HOME}")
    else()
        message("Found minizip at ${minizip_PATH}")
    endif()

    # verify we can link minizip
    set(CMAKE_REQUIRED_INCLUDES "${ZIP_HOME}/include")
    set(CMAKE_REQUIRED_LIBRARIES ${minizip_PATH} ${z_PATH})
    unset(MINIZIP_HOME_VALID CACHE)
    check_cxx_source_compiles("
        #include <zip.h>
        int main() { auto f = &zipOpen; return 0; }"
        MINIZIP_HOME_VALID)
    unset(CMAKE_REQUIRED_LIBRARIES)
    unset(CMAKE_REQUIRED_INCLUDES)
    if (NOT MINIZIP_HOME_VALID)
        message(FATAL_ERROR "Could not link with minizip from ZIP_HOME ${ZIP_HOME}")
    endif()

    # create interface library pointing to the external minizip
    add_library(minizip INTERFACE)
    target_include_directories(minizip INTERFACE "${ZIP_HOME}/include")
    target_link_libraries(minizip INTERFACE ${minizip_PATH})
elseif(ENABLE_ZIP)
    message("ZIP_HOME not specified, so zlib and minizip will be built")
    coda_fetch_driver(
        NAME ${TARGET_NAME}
        ARCHIVE "zlib-1.2.11.tar"
        HASH "SHA256=FAC54AC78F4DED16C64AEC1A9C7D73BC082305306AF849A16787D7828FABA783")

    if (MSVC)
        add_compile_options(/wd4996) # '...': The POSIX name for this item is deprecated. Instead, use the ISO C and C++ conformant name: .... See online help for details.
    endif()

    set(SOURCE_DIR "${${CMAKE_PROJECT_NAME}_${TARGET_NAME}_SOURCE_DIR}")
    foreach(src "adler32.c" "crc32.c" "deflate.c" "infback.c" "inffast.c"
                "inflate.c" "inftrees.c" "trees.c" "zutil.c" "compress.c"
                "uncompr.c" "gzclose.c" "gzlib.c" "gzread.c" "gzwrite.c")
        list(APPEND SOURCES "${SOURCE_DIR}/${src}")
    endforeach()
    add_library(${TARGET_NAME} ${SOURCES})
    target_include_directories(${TARGET_NAME}
        PUBLIC "$<BUILD_INTERFACE:${SOURCE_DIR}>"
               "$<INSTALL_INTERFACE:${CODA_STD_PROJECT_INCLUDE_DIR}>")

    foreach(src "ioapi.c" "zip.c")
        list(APPEND MINIZIP_SOURCES "${SOURCE_DIR}/contrib/minizip/${src}")
    endforeach()
    add_library(minizip ${MINIZIP_SOURCES})
    target_link_libraries(minizip PUBLIC z)
    target_include_directories(${TARGET_NAME}
        PUBLIC "$<BUILD_INTERFACE:${SOURCE_DIR}>"
               "$<BUILD_INTERFACE:${SOURCE_DIR}/contrib/minizip>"
               "$<INSTALL_INTERFACE:${CODA_STD_PROJECT_INCLUDE_DIR}>")

    install(FILES "${SOURCE_DIR}/zconf.h"
                  "${SOURCE_DIR}/zlib.h"
                  "${SOURCE_DIR}/contrib/minizip/zip.h"
                  "${SOURCE_DIR}/contrib/minizip/ioapi.h"
            DESTINATION "${CODA_STD_PROJECT_INCLUDE_DIR}"
            ${CODA_INSTALL_OPTION})
endif()

install(TARGETS z minizip
        EXPORT ${CODA_EXPORT_SET_NAME}
        ${CODA_INSTALL_OPTION}
        LIBRARY DESTINATION "${CODA_STD_PROJECT_LIB_DIR}"
        ARCHIVE DESTINATION "${CODA_STD_PROJECT_LIB_DIR}")

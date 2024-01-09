set(TARGET_NAME z)

coda_fetch_driver(
    NAME ${TARGET_NAME}
    ARCHIVE "zlib-1.2.13.tar"
    HASH "SHA256=A47CDCD8863424356B893B259CB57081EFDCC7FA3C2EB56CA1E881324958A2A9")

set(SOURCE_DIR "${${CMAKE_PROJECT_NAME}_${TARGET_NAME}_SOURCE_DIR}")
# remove crypt.h due to name clash with glibc
file(REMOVE "${SOURCE_DIR}/contrib/minizip/crypt.h")
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

# disable include of crypt.h in minizip to fix name clash issue with glibc
target_compile_definitions(minizip PUBLIC -DNOCRYPT -DNOUNCRYPT)

install(TARGETS z minizip
        EXPORT ${CODA_EXPORT_SET_NAME}
        ${CODA_INSTALL_OPTION}
        LIBRARY DESTINATION "${CODA_STD_PROJECT_LIB_DIR}"
        ARCHIVE DESTINATION "${CODA_STD_PROJECT_LIB_DIR}")

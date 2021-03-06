set(TARGET_NAME jpeg)

if (JPEG_HOME)
    find_library(${TARGET_NAME}_PATH
                 ${TARGET_NAME}
                 PATHS ${JPEG_HOME}
                 PATH_SUFFIXES lib lib64
                 NO_DEFAULT_PATH)
    if ("${${TARGET_NAME}_PATH}" STREQUAL "${TARGET_NAME}_PATH-NOTFOUND")
        message(FATAL_ERROR "jpeg not found at JPEG_HOME ${JPEG_HOME}")
    else()
        message("Found jpeg at ${${TARGET_NAME}_PATH}")
    endif()

    # verify we can link the found library
    set(CMAKE_REQUIRED_INCLUDES "${JPEG_HOME}/include")
    set(CMAKE_REQUIRED_LIBRARIES ${${TARGET_NAME}_PATH})
    unset(JPEG_HOME_VALID CACHE)
    check_cxx_source_compiles("
        #include <stddef.h>
        #include <stdio.h>
        #include <jpeglib.h>
        int main() { void *p = (void*)(jpeg_destroy_decompress); return 0; }"
        JPEG_HOME_VALID)
    unset(CMAKE_REQUIRED_LIBRARIES)
    unset(CMAKE_REQUIRED_INCLUDES)
    if (NOT JPEG_HOME_VALID)
        message(FATAL_ERROR "Could not link with jpeg from JPEG_HOME ${JPEG_HOME}")
    endif()

    # create interface library pointing to the external library
    add_library(${TARGET_NAME} INTERFACE)
    target_include_directories(${TARGET_NAME} INTERFACE "${JPEG_HOME}/include")
    target_link_libraries(${TARGET_NAME} INTERFACE ${${TARGET_NAME}_PATH})

else()
    message("JPEG_HOME not specified, so it will be built")

    coda_fetch_driver(
        NAME ${TARGET_NAME}
        ARCHIVE "jpegsrc.v9.tar"
        HASH "SHA256=650de9b9c73b627c365c2c1617436bfdef47decddc1c6f7e006bd7655c4f8111"
    )
    set(HAVE_PROTOTYPES 1)
    check_cxx_source_compiles("int main() { unsigned short; return 0; }"
                                HAVE_UNSIGNED_SHORT)
    check_cxx_source_compiles("int main() { unsigned char; return 0; }"
                                HAVE_UNSIGNED_CHAR)
    check_include_file("stddef.h" HAVE_STDDEF_H)
    check_include_file("stdlib.h" HAVE_STDLIB_H)
    check_include_file("locale.h" HAVE_LOCALE_H)

    set(SOURCE_DIR "${${CMAKE_PROJECT_NAME}_${TARGET_NAME}_SOURCE_DIR}")
    foreach(src "cdjpeg.c"   "jaricom.c"  "jcapimin.c" "jcapistd.c"
                "jcarith.c"  "jccoefct.c" "jccolor.c"  "jcdctmgr.c"
                "jchuff.c"   "jcinit.c"   "jcmainct.c" "jcmarker.c"
                "jcmaster.c" "jcomapi.c"  "jcparam.c"  "jcprepct.c"
                "jcsample.c" "jctrans.c"  "jdapimin.c" "jdapistd.c"
                "jdarith.c"  "jdatadst.c" "jdatasrc.c" "jdcoefct.c"
                "jdcolor.c"  "jddctmgr.c" "jdhuff.c"   "jdinput.c"
                "jdmainct.c" "jdmarker.c" "jdmaster.c" "jdmerge.c"
                "jdpostct.c" "jdsample.c" "jdtrans.c"  "jerror.c"
                "jfdctflt.c" "jfdctfst.c" "jfdctint.c" "jidctflt.c"
                "jidctfst.c" "jidctint.c" "jmemmgr.c"  "jmemnobs.c"
                "jquant1.c"  "jquant2.c"  "jutils.c"   "rdbmp.c"
                "rdcolmap.c" "rdgif.c"    "rdppm.c"    "rdrle.c"
                "rdswitch.c" "rdtarga.c"  "transupp.c" "wrbmp.c"
                "wrgif.c"    "wrppm.c"    "wrrle.c"    "wrtarga.c")
        list(APPEND SOURCES "${SOURCE_DIR}/${src}")
    endforeach()

    set(JCONFIG "${${CMAKE_PROJECT_NAME}_${TARGET_NAME}_BINARY_DIR}/jconfig.h")
    configure_file("jconfig.h.in" ${JCONFIG})
    add_library(${TARGET_NAME} ${SOURCES})
    target_include_directories(${TARGET_NAME}
        PUBLIC "$<BUILD_INTERFACE:${SOURCE_DIR}>"
               "$<BUILD_INTERFACE:${${CMAKE_PROJECT_NAME}_${TARGET_NAME}_BINARY_DIR}>"
               "$<INSTALL_INTERFACE:${CODA_STD_PROJECT_INCLUDE_DIR}>")

    install(FILES "${SOURCE_DIR}/jpeglib.h"
                  "${SOURCE_DIR}/jmorecfg.h"
                  "${SOURCE_DIR}/jerror.h"
                  "${JCONFIG}"
            DESTINATION "${CODA_STD_PROJECT_INCLUDE_DIR}"
            ${CODA_INSTALL_OPTION})
endif()
install(TARGETS ${TARGET_NAME}
        EXPORT ${CODA_EXPORT_SET_NAME}
        ${CODA_INSTALL_OPTION}
        LIBRARY DESTINATION "${CODA_STD_PROJECT_LIB_DIR}"
        ARCHIVE DESTINATION "${CODA_STD_PROJECT_LIB_DIR}")

set(TARGET_NAME jpeg)

if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    # use this CMakeLists.txt to build and package libjpeg with conan
    project(coda-oss_jpeg)
    include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    conan_basic_setup()
    set(CODA_SKIP_SYSTEM_DEPENDENCIES ON)
    include(CodaBuild.cmake)
    coda_initialize_build()
    coda_generate_package_config()
endif()

coda_fetch_driver(
    NAME ${TARGET_NAME}
    ARCHIVE "jpegsrc.v9d.tar"
    HASH "SHA256=898C46BFE9152D51220C6679E905DE4DF75D2A67DCF315BF80459673AB1EB797"
)
set(HAVE_PROTOTYPES 1)
set(HAVE_UNSIGNED_SHORT true)
set(HAVE_UNSIGNED_CHAR true)
set(HAVE_STDDEF_H true)
set(HAVE_STDLIB_H true)
set(HAVE_STDLIB_H true)

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

install(TARGETS ${TARGET_NAME}
        EXPORT ${CODA_EXPORT_SET_NAME}
        ${CODA_INSTALL_OPTION}
        LIBRARY DESTINATION "${CODA_STD_PROJECT_LIB_DIR}"
        ARCHIVE DESTINATION "${CODA_STD_PROJECT_LIB_DIR}")

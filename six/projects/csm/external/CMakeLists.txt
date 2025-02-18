set(TARGET_NAME CSMAPI)
string(TOLOWER ${TARGET_NAME} target_name_lc)

coda_fetch_driver(
        NAME ${TARGET_NAME}
        ARCHIVE "csm_v3.1.0.zip"
        HASH "SHA256=7c2359f4ec911cedc3bbf6221f8dbe0914479c9af8184f0d79ecc60f6c1d5ad3"
    )

set("${target_name_lc}_VERSION" "310" CACHE INTERNAL "version string for ${target_name_lc}")
set(SOURCE_DIR "${${CMAKE_PROJECT_NAME}_${target_name_lc}_SOURCE_DIR}")

file(GLOB SOURCES "${SOURCE_DIR}/*.cpp")
set(CODA_EXPORT_SET_NAME "${CMAKE_PROJECT_NAME}Targets")

add_library(${TARGET_NAME} SHARED ${SOURCES})
target_include_directories(${TARGET_NAME} INTERFACE
    $<BUILD_INTERFACE:${SOURCE_DIR}>
    $<INSTALL_INTERFACE:>)
install(TARGETS ${TARGET_NAME})
install(TARGETS ${TARGET_NAME} EXPORT ${CODA_EXPORT_SET_NAME})

# The static version of CSMAPI exists only to support unit tests on the plugin.
# See the comments in the CMakeLists.txt file in the parent directory.  Using
# this version for any other purpose will almost surely break the plugin
# functionality.
add_library(${TARGET_NAME}-static ${SOURCES})
target_include_directories(${TARGET_NAME}-static INTERFACE
    $<BUILD_INTERFACE:${SOURCE_DIR}>
    $<INSTALL_INTERFACE:>)
install(TARGETS ${TARGET_NAME}-static EXPORT ${CODA_EXPORT_SET_NAME})


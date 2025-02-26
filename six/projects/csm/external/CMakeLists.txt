if (MSVC)
    # This is not our code, so we are not going to change it to eliminate these problems
    add_compile_options(/wd4251) # class '...' needs to have dll-interface to be used by clients of class '...'
    add_compile_options(/wd4275) # non dll-interface class '...' used as base for dll-interface class '...'
    add_compile_options(/wd4267) # conversion from '...' to '...'
    add_compile_options(/wd4996) # '...': This function or variable may be unsafe
    add_compile_options(/wd4244) # 'return': conversion from '...' to '...'
    add_compile_options(/wd4005) # '...': macro redefinition
endif()

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

if (MSVC)
    add_library(${TARGET_NAME} SHARED ${SOURCES})
else()
    add_library(${TARGET_NAME} INTERFACE)
endif()
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
if (MSVC)
    # Define CSM_LIBRARY so the symbols are not marked as being imported from a
    # shared library/dll.  Fixes Windows link problems where they are expected
    # to be defined externally.
    target_compile_options(${TARGET_NAME}-static PUBLIC /DCSM_LIBRARY)
endif()
install(TARGETS ${TARGET_NAME}-static EXPORT ${CODA_EXPORT_SET_NAME})


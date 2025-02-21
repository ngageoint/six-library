if (MSVC)
    # We are including headers from an external library (CSMAPI) and we can't change it to fix these problems
    add_compile_options(/wd4251) # class '...' needs to have dll-interface to be used by clients of class '...'
    add_compile_options(/wd4275) # non dll-interface class '...' used as base for dll-interface class '...'
    add_compile_options(/wd4267) # conversion from '...' to '...'
    add_compile_options(/wd4996) # '...': This function or variable may be unsafe
    add_compile_options(/wd4244) # 'return': conversion from '...' to '...'
endif()

add_subdirectory(external)

set(MODULE_NAME csm-plugin)

# construct final plugin filename
file(READ "source/SICDSensorModel.cpp" version_source)
string(REGEX MATCH "SICDSensorModel::VERSION\\(([0-9]), ([0-9]), ([0-9])\\)" _ ${version_source})
set(PLUGIN_VERSION "${CMAKE_MATCH_1}${CMAKE_MATCH_2}${CMAKE_MATCH_3}")

if ("${PLUGIN_VERSION}" STREQUAL "")
    message(FATAL_ERROR "Could not extract CSM plugin version string from source code.")
endif()

file(READ "source/SIDDSensorModel.cpp" version_source)
string(REGEX MATCH "SIDDSensorModel::VERSION\\(([0-9]), ([0-9]), ([0-9])\\)" _ ${version_source})
set(PLUGIN_VERSION2 "${CMAKE_MATCH_1}${CMAKE_MATCH_2}${CMAKE_MATCH_3}")

if (NOT "${PLUGIN_VERSION}" STREQUAL "${PLUGIN_VERSION2}")
    message(FATAL_ERROR "CSM plugin SICD and SIDD sensor model version numbers should match.")
endif()

message("Extracted CSM plugin version string ${PLUGIN_VERSION} from source")

if("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    set(PLATFORM_NAME "linux")
elseif("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
    set(PLATFORM_NAME "win")
else()
    message(FATAL_ERROR "Platform not recognized.  Unable to determine CSM plugin filename.")
endif()

set(PLUGIN_FILENAME "SIX_SAR_ARKA_${PLUGIN_VERSION}_${PLATFORM_NAME}_CSM${csmapi_VERSION}")

# Build two libraries:
#
# (1) A shared library which is the CSM plugin itself.  This receives a special
# name which is the delivered file name.
#
# (2) A static library, which is to only be used for unit testing.
file(GLOB SOURCES source/*.cpp)

# Shared library version (the CSM plugin)
#
# This cannot use coda_add_module because we need to set the final filename so
# it is not libcsm-plugin.so.  This means some of these are duplicated from
# CodaBuild.cmake.
add_library(${MODULE_NAME} SHARED ${SOURCES})
target_link_libraries(${MODULE_NAME} scene-c++ six.sicd-c++ six.sidd-c++ CSMAPI)
target_include_directories(${MODULE_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
set_target_properties(${MODULE_NAME} PROPERTIES PREFIX "" OUTPUT_NAME "${PLUGIN_FILENAME}")

# Manually need to create install target
set(CODA_EXPORT_SET_NAME "${CMAKE_PROJECT_NAME}Targets")
install(TARGETS ${MODULE_NAME}
        EXPORT ${CODA_EXPORT_SET_NAME}
        LIBRARY DESTINATION "share/CSM/plugins/")

# Static library version
#
# The unit test is statically linked against the CSMAPI and the SIX CSM
# functionality.  This is the only use for the static version of those two
# libraries.  By statically linking them, the unit test can run without any
# problems encountered in needing to specify where the two plugins (CSMAPI and
# SIX_CSM) exist.
set(TARGET_LANGUAGE c++)
coda_add_module(
    ${MODULE_NAME}-static
    SOURCES ${SOURCES}
    DEPS scene-c++ six.sicd-c++ six.sidd-c++ CSMAPI-static)
coda_add_tests(
    MODULE_NAME ${MODULE_NAME}-static
    DIRECTORY "tests")
coda_add_tests(
    MODULE_NAME ${MODULE_NAME}-static
    DIRECTORY "unittests"
    UNITTEST)

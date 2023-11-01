# this file contains common functions and macros for configuring and adding
# components to the build
#
#   coda_initialize_build               - set up global configuration
#   coda_generate_package_config        - generate CMake module used to import
#                                         targets into downstream projects
#   coda_fetch_driver                   - external dependencies
#   coda_add_tests                      - tests/unittests for C or C++ modules
#   coda_generate_module_config_header  - generate headers for C or C++ modules
#   coda_add_module                     - C or C++ modules
#   coda_add_plugin                     - dynamic library plugins

include(CheckCXXSourceCompiles)
include(CheckIncludeFile)
include(CheckLibraryExists)
include(CheckSymbolExists)
include(CheckTypeSize)
include(ExternalProject)
include(FetchContent) # Requires CMake 3.11+
include(TestBigEndian)

# print out some compile options/flags, for debugging purposes
function(coda_show_compile_options)
    # show the compile options for the project directory
    get_property(tmp_compile_options DIRECTORY "./" PROPERTY COMPILE_OPTIONS)
    message("COMPILE OPTIONS=${tmp_compile_options}")
    unset(tmp_compile_options)

    # show the global compile flags
    foreach(suffix "" "_DEBUG" "_RELWITHDEBINFO" "_RELEASE")
        message("CMAKE_C_FLAGS${suffix}=${CMAKE_C_FLAGS${suffix}}")
        message("CMAKE_CXX_FLAGS${suffix}=${CMAKE_CXX_FLAGS${suffix}}")
    endforeach()
endfunction()

# set the appropriate CRT link flags for MSVC builds
macro(coda_setup_msvc_crt)
    if (CONAN_PACKAGE_NAME)
        # conan handles this
    else()
        set(CODA_MSVC_RUNTIME "/MD")
        foreach(build_config DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
            string(REGEX REPLACE "/M[DT]" "${CODA_MSVC_RUNTIME}" CMAKE_CXX_FLAGS_${build_config} "${CMAKE_CXX_FLAGS_${build_config}}")
            string(REGEX REPLACE "/M[DT]" "${CODA_MSVC_RUNTIME}" CMAKE_C_FLAGS_${build_config} "${CMAKE_C_FLAGS_${build_config}}")
        endforeach()
    endif()
endmacro()

# Set up the global build configuration
macro(coda_initialize_build)
    # Standard directory names used throughout the project.
    set(CODA_STD_PROJECT_INCLUDE_DIR    "include")
    set(CODA_STD_PROJECT_LIB_DIR        "lib")
    set(CODA_STD_PROJECT_BIN_DIR        "bin")

    # Detect 32/64-bit architecture
    if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
        message(FATAL_ERROR "Unexpected Pointer Size: ${CMAKE_SIZEOF_VOID_P} Bytes")
    endif()

    if (NOT MSVC)
        # default configuration is RelWithDebInfo for non-MSVC builds
        # MSVC is a multi-configuration generator, so configuration is not
        # known until build time (CMAKE_BUILD_TYPE is never set, must not be
        # relied on)
        set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Select Build Type")
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
            "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
    endif()

    enable_testing()

    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    # MSVC-specific flags and options.
    if (MSVC)
        set_property(GLOBAL PROPERTY USE_FOLDERS ON)

        coda_setup_msvc_crt()

        # catch exceptions that bubble through a C-linkage layer
        string(REGEX REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        add_compile_options(/EHs /bigobj)

        add_definitions(
            -DWIN32_LEAN_AND_MEAN
            -DNOMINMAX
            -D_USE_MATH_DEFINES
        )

        link_libraries(    # CMake uses this for both libraries and linker options
            -STACK:80000000
        )

        # This should probably be replaced by GenerateExportHeader
        #set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
        set(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD TRUE)

       if (ENABLE_ASAN)
            # https://docs.microsoft.com/en-us/cpp/sanitizers/asan?view=msvc-160
            add_compile_options(/fsanitize=address)
       endif()

       # Note SSE2 is implicitly enabled for x64 builds.
       if (ENABLE_AVX2 AND (NOT ENABLE_AVX512F))
            # https://learn.microsoft.com/en-us/cpp/build/reference/arch-x86?view=msvc-170
            add_compile_options(/arch:AVX2)
       endif()
       if (ENABLE_AVX512F)
            # https://learn.microsoft.com/en-us/cpp/build/reference/arch-x86?view=msvc-170
           add_compile_options(/arch:AVX512)
       endif()

    endif()

    # Unix/Linux specific options
    if (UNIX)
        add_definitions(
            -D_LARGEFILE_SOURCE
            -D_FILE_OFFSET_BITS=64
        )

       if (ENABLE_ASAN)
            # https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html
            add_compile_options(-fsanitize=address)
            add_link_options(-fsanitize=address)
       endif()

       # Note SSE2 is implicitly enabled for x64 builds.
       if (ENABLE_AVX2 AND (NOT ENABLE_AVX512F))
            # https://gcc.gnu.org/onlinedocs/gcc/x86-Options.html
            # It doesn't look like GCC has a specific option for AVX2;
            # other projects use "haswell"
            add_compile_options(-march=haswell)
       endif()
       if (ENABLE_AVX512F)
            # https://gcc.gnu.org/onlinedocs/gcc/x86-Options.html
            # It doesn't look like GCC has a specific option for AVX512F;
            # other projects use "native" which isn't quite correct.'
            add_compile_options(-march=native)
       endif()

    endif()

    # all targets should be installed using this export set
    set(CODA_EXPORT_SET_NAME "${CMAKE_PROJECT_NAME}Targets")

    if (NOT CODA_SKIP_SYSTEM_DEPENDENCIES)
        include(CodaFindSystemDependencies)
        coda_find_system_dependencies()
    endif()
    coda_show_compile_options()
endmacro()


# Utility to filter a list of files.
#
# dest_name     - Destination variable name in parent's scope
# file_list     - Input list of files (possibly with paths)
# filter_list   - Input list of files to filter out
#                   (must be bare filenames; no paths)
#
function(filter_files dest_name file_list filter_list)
    foreach(test_src ${file_list})
        get_filename_component(test_src_name ${test_src} NAME)
        if (NOT ${test_src_name} IN_LIST filter_list)
            list(APPEND good_names ${test_src})
        endif()
    endforeach()
    set(${dest_name} ${good_names} PARENT_SCOPE)
endfunction()


# Generate a package config file to help downstream projects import our targets.
#
# The package can be loaded by calling find_package(package_name REQUIRED) after
# adding the package config file directory to CMAKE_PREFIX_PATH.
#
# All arguments are forwarded as PATH_VARS to configure_package_config_file.
function(coda_generate_package_config)
    # only run if we are the top level project
    if ("${CMAKE_PROJECT_NAME}" STREQUAL "${PROJECT_NAME}")
        # generate a CMake module to import this project's targets into downstream projects
        install(EXPORT ${CODA_EXPORT_SET_NAME}
                DESTINATION "${CODA_STD_PROJECT_LIB_DIR}/cmake")

        # create a wrapper module for the above to allow additional configuration
        include(CMakePackageConfigHelpers)
        configure_package_config_file(
            "cmake/${CMAKE_PROJECT_NAME}Config.cmake.in"
            "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake"
            INSTALL_DESTINATION "lib/cmake"
            PATH_VARS ${ARGN}
        )
        #write_basic_package_version_file(
        #    ${CMAKE_CURRENT_BINARY_DIR}/FooConfigVersion.cmake
        #    VERSION 1.2.3
        #    COMPATIBILITY SameMajorVersion )
        install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake"
                #"${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake"
                DESTINATION "lib/cmake")
    endif()
endfunction()


# Add a driver (3rd-party library) to the build.
#
# Single value arguments:
#   NAME        - Name of the driver
#   ARCHIVE     - Location of the archive containing the driver.
#                 This can be a path relative to ${CMAKE_CURRENT_SOURCE_DIR}.
#   HASH        - hash signature in the form hashtype=hashvalue
#
#  The 3P's source and build directories will be stored in
#       ${${target_name_lc}_SOURCE_DIR}, and
#       ${${target_name_lc}_BINARY_DIR} respectively,
#
#       where ${target_name_lc} is the lower-cased target name.
#
function(coda_fetch_driver)
    cmake_parse_arguments(
        ARG                          # prefix
        ""                           # options
        "NAME;ARCHIVE;HASH"          # single args
        ""                           # multi args
        "${ARGN}"
    )
    if (ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "received unexpected argument(s): ${ARG_UNPARSED_ARGUMENTS}")
    endif()

    set(target_name ${CMAKE_PROJECT_NAME}_${ARG_NAME})
    # Use 'FetchContent' to download and unpack the files.  Set it up here.
    FetchContent_Declare(${target_name}
        URL "${CMAKE_CURRENT_SOURCE_DIR}/${ARG_ARCHIVE}"
        URL_HASH ${ARG_HASH}
    )
    FetchContent_GetProperties(${target_name})
    # The returned properties use the lower-cased name
    string(TOLOWER ${target_name} target_name_lc)
    if (NOT ${target_name_lc}_POPULATED) # This makes sure we only fetch once.
        message("Populating content for external dependency ${driver_name}")
        # Now (at configure time) unpack the content.
        FetchContent_Populate(${target_name})
        # Remember where we put stuff
        set("${target_name_lc}_SOURCE_DIR" "${${target_name_lc}_SOURCE_DIR}"
            CACHE INTERNAL "source directory for ${target_name_lc}")
        set("${target_name_lc}_BINARY_DIR" "${${target_name_lc}_BINARY_DIR}"
            CACHE INTERNAL "binary directory for ${target_name_lc}")
    endif()
endfunction()


# Add a module's tests or unit tests to the build
#
# Single value arguments:
#   MODULE_NAME     - Name of the module
#   DIRECTORY       - Directory containing the tests' source code, relative to
#                     the current source directory. All source files beneath
#                     this directory will be used. Each source file is assumed
#                     to create a separate executable.
#
# Multi value arguments:
#   DEPS            - Modules that the tests are dependent upon.
#   SOURCES         - List of test source files, one for each test. If not
#                     provided, the specified directory will be globbed for
#                     source files.
#   ARGS            - Additional command line arguments to pass to unit tests,
#                     may help in locating input data files.
#   FILTER_LIST     - Source files to ignore
#
# Option arguments:
#   UNITTEST        - If present, the test will be added to the CTest suite for
#                     automated running.
#
function(coda_add_tests)
    cmake_parse_arguments(
        ARG                         # prefix
        "UNITTEST"                  # options
        "MODULE_NAME;DIRECTORY"     # single args
        "DEPS;SOURCES;ARGS;FILTER_LIST"  # multi args
        "${ARGN}"
    )
    if (ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "received unexpected argument(s): ${ARG_UNPARSED_ARGUMENTS}")
    endif()

    if (NOT ARG_DIRECTORY)
        message(FATAL_ERROR "Must give a test directory")
    endif()
    if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${ARG_DIRECTORY}")
        message(FATAL_ERROR "Directory ${CMAKE_CURRENT_SOURCE_DIR}/${ARG_DIRECTORY} does not exist")
    endif()

    if (ARG_SOURCES)
        foreach(src ${ARG_SOURCES})
            list(APPEND local_tests "${ARG_DIRECTORY}/${src}")
        endforeach()
    else()
        # Find all the source files, relative to the module's directory
        file(GLOB_RECURSE local_tests RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
                "${ARG_DIRECTORY}/*.cpp")
    endif()
    # Filter out ignored files
    filter_files(local_tests "${local_tests}" "${ARG_FILTER_LIST}")

    # make a group target to build all tests for the current module
    set(test_group_tgt "${ARG_MODULE_NAME}_tests")
    if (NOT TARGET ${test_group_tgt})
        add_custom_target(${test_group_tgt})
    endif()

    list(APPEND ARG_DEPS ${ARG_MODULE_NAME}-${TARGET_LANGUAGE} TestCase)

    # get all interface libraries and include directories from the dependencies
    foreach(dep ${ARG_DEPS})
        if (TARGET ${dep})
            get_property(dep_includes TARGET ${dep}
                            PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
            list(APPEND include_dirs ${dep_includes})
        endif()
    endforeach()

    foreach(test_src ${local_tests})
        # Use the base name of the source file as the name of the test
        get_filename_component(test_name "${test_src}" NAME_WE)
        set(test_target "${ARG_MODULE_NAME}_${test_name}")
        add_executable(${test_target} "${test_src}")
        set_target_properties(${test_target} PROPERTIES OUTPUT_NAME ${test_name})
        add_dependencies(${test_group_tgt} ${test_target})
        get_filename_component(test_dir "${test_src}" DIRECTORY)
        # Do a bit of path manipulation to make sure tests in deeper subdirs
        # retain those subdirs in their build outputs.
        file(RELATIVE_PATH test_subdir
                "${CMAKE_CURRENT_SOURCE_DIR}/${ARG_DIRECTORY}"
                "${CMAKE_CURRENT_SOURCE_DIR}/${test_dir}")

        # Set IDE subfolder so that tests appear in their own tree
        set_target_properties(${test_target}
            PROPERTIES FOLDER "${ARG_DIRECTORY}/${ARG_MODULE_NAME}/${test_subdir}")

        target_link_libraries(${test_target} PRIVATE ${ARG_DEPS})
        target_include_directories(${test_target} PRIVATE ${include_dirs})

        # add unit tests to automatic test suite
        if (${ARG_UNITTEST})
            add_test(NAME ${test_target} COMMAND ${test_target} ${ARG_ARGS})
        endif()

        # Install [unit]tests to separate subtrees
        install(TARGETS ${test_target}
                ${CODA_INSTALL_OPTION}
                RUNTIME DESTINATION "${ARG_DIRECTORY}/${ARG_MODULE_NAME}/${test_subdir}")
    endforeach()
endfunction()


# Generate a configuration header file for a coda module.
#
# Arguments:
#   MODULE_NAME     - Name of the module to which the configuration file belongs
#
function(coda_generate_module_config_header MODULE_NAME)
    # Periods in target names for dirs are replaced with slashes (subdirectories).
    string(REPLACE "." "/" tgt_munged_dirname ${MODULE_NAME})

    # Periods in target names for files are replaced with underscores.
    # Note that this variable name is used in the *.cmake.in files.
    string(REPLACE "." "_" tgt_munged_name ${MODULE_NAME})

    # If we find a *_config.h.cmake.in file, generate the corresponding *_config.h, and put the
    #   target directory in the include path.
    set(config_file_template "${CMAKE_CURRENT_SOURCE_DIR}/${CODA_STD_PROJECT_INCLUDE_DIR}/${tgt_munged_dirname}/${MODULE_NAME}_config.h.cmake.in")
    if (EXISTS ${config_file_template})
        set(config_file_out "${CODA_STD_PROJECT_INCLUDE_DIR}/${tgt_munged_dirname}/${tgt_munged_name}_config.h")
        message(STATUS "Processing config header: ${config_file_template} -> ${config_file_out}")
        configure_file("${config_file_template}" "${config_file_out}")
        install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${config_file_out}"
                DESTINATION "${CODA_STD_PROJECT_INCLUDE_DIR}/${tgt_munged_dirname}"
                ${CODA_INSTALL_OPTION})
    endif()
endfunction()


# Add a C or C++ module to the build
#
# The CMake target for the module is named "${MODULE_NAME}-${TARGET_LANGUAGE}"
#
# Positional arguments:
#   MODULE_NAME     - Name of the module
#
# Single value arguments:
#   VERSION         - Version of the module (currently unused)
#
# Multi value arguments:
#   DEPS            - List of linkable dependencies for the library
#   SOURCES         - List of source files to link into this library. If not
#                     provided, the source subdirectory will be globbed for
#                     source files.
#   SOURCE_FILTER   - Source files to ignore
#
# Implicit arguments from parent scope:
#   TARGET_LANGUAGE - module target language (c or c++)
#
function(coda_add_module MODULE_NAME)
    cmake_parse_arguments(
        ARG                                 # prefix
        ""                                  # options
        "VERSION"                           # single args
        "DEPS;SOURCES;SOURCE_FILTER"        # multi args
        "${ARGN}"
    )
    if (ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "received unexpected argument(s): ${ARG_UNPARSED_ARGUMENTS}")
    endif()
    if (NOT TARGET_LANGUAGE)
        message(FATAL_ERROR "must set TARGET_LANGUAGE before calling this function (c or c++)")
    endif()
    set(target_name "${MODULE_NAME}-${TARGET_LANGUAGE}")

    if (ARG_SOURCES)
        set(local_sources ${ARG_SOURCES})
    else()
        # Find all the source files, relative to the module's directory
        file(GLOB_RECURSE local_sources RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "source/*.cpp" "source/*.c")

        if (GPU_ENABLED AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/kernels")
            file(GLOB_RECURSE cuda_sources RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "kernels/*.cu")
            list(APPEND local_sources ${cuda_sources})
        endif()
    endif()

    # Filter out ignored files
    filter_files(local_sources "${local_sources}" "${ARG_SOURCE_FILTER}")

    if (NOT local_sources)
        # Libraries without sources must be declared to CMake as INTERFACE libraries.
        # When "linked" to another target, their interface include directories
        # and interface link libraries are propagated.
        set(lib_type INTERFACE)
        add_library(${target_name} INTERFACE)
    else()
        set(lib_type PUBLIC)
        add_library(${target_name} ${local_sources})
    endif()

    target_link_libraries(${target_name} ${lib_type} ${ARG_DEPS})

    target_include_directories(${target_name} ${lib_type}
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${CODA_STD_PROJECT_INCLUDE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/${CODA_STD_PROJECT_INCLUDE_DIR}>"    # for generated include headers
        "$<INSTALL_INTERFACE:${CODA_STD_PROJECT_INCLUDE_DIR}>")

    if (cuda_sources)
        get_property(tmp TARGET ${target_name} PROPERTY COMPILE_OPTIONS)
        set_target_properties(${target_name} PROPERTIES
            CUDA_SEPARABLE_COMPILATION ON
            COMPILE_OPTIONS $<IF:$<STREQUAL:$<COMPILE_LANGUAGE>,CUDA>,-rdc=true,>
        )
    endif()

    # Set up install destinations for binaries
    install(TARGETS ${target_name}
            EXPORT ${CODA_EXPORT_SET_NAME}
            ${CODA_INSTALL_OPTION}
            LIBRARY DESTINATION "${CODA_STD_PROJECT_LIB_DIR}"
            ARCHIVE DESTINATION "${CODA_STD_PROJECT_LIB_DIR}"
            RUNTIME DESTINATION "${CODA_STD_PROJECT_BIN_DIR}")

    # Set up install destination for headers
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${CODA_STD_PROJECT_INCLUDE_DIR}")
        install(DIRECTORY "${CODA_STD_PROJECT_INCLUDE_DIR}/"
                DESTINATION "${CODA_STD_PROJECT_INCLUDE_DIR}/"
                ${CODA_INSTALL_OPTION}
                FILES_MATCHING
                    PATTERN "*.in" EXCLUDE
                    PATTERN "*")
    endif()

    # install conf directory, if present
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/conf")
        install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/conf"
                DESTINATION "share/${MODULE_NAME}"
                ${CODA_INSTALL_OPTION})
    endif()

endfunction()

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
#   coda_add_swig_python_module         - SWIG Python modules

include(CheckCXXSourceCompiles)
include(CheckIncludeFile)
include(CheckLibraryExists)
include(CheckSymbolExists)
include(CheckTypeSize)
include(ExternalProject)
include(FetchContent) # Requires CMake 3.11+
include(TestBigEndian)

if (WIN32 AND BUILD_SHARED_LIBS)
    include(GenerateExportHeader)
endif()

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

    message("CMAKE_SWIG_FLAGS=${CMAKE_SWIG_FLAGS}")
endfunction()

# set the appropriate CRT link flags for MSVC builds
macro(coda_setup_msvc_crt)
    set(STATIC_CRT OFF CACHE BOOL "use static CRT library /MT, or /MTd for Debug (/MD or /MDd if off)")
    if (CONAN_LINK_RUNTIME MATCHES "MT") # will also match MTd
        set(STATIC_CRT ON CACHE BOOL "" FORCE)
    endif()
    if (STATIC_CRT)
        set(CODA_MSVC_RUNTIME "/MT")
    else()
        set(CODA_MSVC_RUNTIME "/MD")
    endif()
    foreach(build_config DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
        string(REGEX REPLACE "/M[DT]" "${CODA_MSVC_RUNTIME}" CMAKE_CXX_FLAGS_${build_config} "${CMAKE_CXX_FLAGS_${build_config}}")
        string(REGEX REPLACE "/M[DT]" "${CODA_MSVC_RUNTIME}" CMAKE_C_FLAGS_${build_config} "${CMAKE_C_FLAGS_${build_config}}")
    endforeach()
endmacro()

# Set up the global build configuration
macro(coda_initialize_build)
    option(CODA_PARTIAL_INSTALL "Allow building and installing a subset of the targets" OFF)
    if (CODA_PARTIAL_INSTALL)
        # This setting, along with setting all install commands as "OPTIONAL",
        # allows installing a subset of the targets. It is off by default
        # because it forces the developer to run separate build and install
        # steps, and could result in installing stale targets if code is updated
        # and install is run without rebuilding.
        #
        # CMake's default behavior forces everything to be built when the
        # install target is run. This override allows one to build a subset of
        # the project and then run the install target to install only what has
        # been built (the build and install must be run as separate steps).
        # For example:
        #
        #   cmake --build . --target target1
        #   cmake --build . --target target2
        #   cmake --build . --target install
        #
        # or in CMake 3.15+
        #   cmake --build . --target target1 target2
        #   cmake --build . --target install
        #
        # to build and install everything, run (with Unix Makefiles)
        #   cmake --build . --target all
        #   cmake --build . --target install
        #
        # and for MSVC:
        #   cmake --build . --target ALL_BUILD
        #   cmake --build . --target install
        #
        # This feature still has some rough edges, in that files and directories
        # that are installed from the source tree are always installed (because they
        # always exist regardless of what was built). This could be fixed by copying
        # them from the source tree to the build directory during the build stage
        # and then pointing the install commands at the file paths within the build
        # directory.
        set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY ON)
        set(CODA_INSTALL_OPTION OPTIONAL)
    endif()

    # Standard directory names used throughout the project.
    set(CODA_STD_PROJECT_INCLUDE_DIR    "include")
    set(CODA_STD_PROJECT_LIB_DIR        "lib")
    set(CODA_STD_PROJECT_BIN_DIR        "bin")

    # Detect 32/64-bit architecture
    if(NOT CODA_BUILD_BITSIZE)
        if (CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(CODA_BUILD_BITSIZE "64" CACHE STRING "Select Architecture" FORCE)
        elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
            set(CODA_BUILD_BITSIZE "32" CACHE STRING "Select Architecture" FORCE)
        else()
            message(FATAL_ERROR "Unexpected Pointer Size: ${CMAKE_SIZEOF_VOID_P} Bytes")
        endif()
        set_property(CACHE CODA_BUILD_BITSIZE PROPERTY STRINGS "64" "32")
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

    option(BUILD_SHARED_LIBS "Build shared libraries instead of static." OFF)
    if(BUILD_SHARED_LIBS)
        set(CODA_LIBRARY_TYPE "shared")
    else()
        set(CODA_LIBRARY_TYPE "static")
    endif()

    option(CODA_BUILD_TESTS "build tests" ON)
    if (CODA_BUILD_TESTS)
        enable_testing()
    endif()

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
    endif()

    # Unix/Linux specific options
    if (UNIX)
        add_definitions(
            -D_LARGEFILE_SOURCE
            -D_FILE_OFFSET_BITS=64
        )
    endif()

    # all targets should be installed using this export set
    set(CODA_EXPORT_SET_NAME "${CMAKE_PROJECT_NAME}Targets")

    include(CodaFindSystemDependencies)
    coda_find_system_dependencies()
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
    if (CODA_BUILD_TESTS)
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
    endif()
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
    install(DIRECTORY "${CODA_STD_PROJECT_INCLUDE_DIR}/"
            DESTINATION "${CODA_STD_PROJECT_INCLUDE_DIR}/"
            ${CODA_INSTALL_OPTION}
            FILES_MATCHING
                PATTERN "*.h"
                PATTERN "*.hpp")

    # install conf directory, if present
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/conf")
        install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/conf"
                DESTINATION "share/${MODULE_NAME}"
                ${CODA_INSTALL_OPTION})
    endif()

    # needs some work to get shared build working on Windows
    #if (WIN32 AND BUILD_SHARED_LIBS AND "${lib_type}" STREQUAL PUBLIC)
    #    generate_export_header(${target_name})
    #    install(FILES ${PROJECT_BINARY_DIR}/${target_name}_export.h
    #            DESTINATION ${CODA_STD_PROJECT_INCLUDE_DIR})
    #endif()
endfunction()


# Add a plugin (dynamically loaded library) to the build
#
# Positional arguments:
#   PLUGIN_NAME     - The name of this plugin
#   MODULE_NAME     - The module associated with this plugin
#
# Single value arguments:
#   VERSION         - Version number of this plugin
#
# Multi value arguments:
#   DEPS            - List of dependencies for the plugin
#   SOURCES         - Optional list of source files for compiling the plugin.
#                     If not provided, the source files will be globbed.
#
function(coda_add_plugin PLUGIN_NAME MODULE_NAME)
    cmake_parse_arguments(
        ARG                         # prefix
        ""                          # options
        "VERSION"                   # single args
        "DEPS;SOURCES"              # multi args
        "${ARGN}"
    )
    if (ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "received unexpected argument(s): ${ARG_UNPARSED_ARGUMENTS}")
    endif()

    set(OUTPUT_NAME "${PLUGIN_NAME}-${TARGET_LANGUAGE}")
    set(TARGET_NAME "${ARG_MODULE_NAME}_${OUTPUT_NAME}")

    if (NOT ARG_SOURCES)
        file(GLOB SOURCES "source/*.cpp" "source/*.c")
    else()
        set(SOURCES "${ARG_SOURCES}")
    endif()

    add_library(${TARGET_NAME} MODULE "${SOURCES}")
    set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME ${OUTPUT_NAME})

    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include")
        target_include_directories(${TARGET_NAME}
            PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                   "$<INSTALL_INTERFACE:${CODA_STD_PROJECT_INCLUDE_DIR}>")
    endif()

    target_link_libraries(${TARGET_NAME} PUBLIC ${ARG_DEPS})

    target_compile_definitions(${TARGET_NAME} PRIVATE PLUGIN_MODULE_EXPORTS)

    install(TARGETS ${TARGET_NAME}
            EXPORT ${CODA_EXPORT_SET_NAME}
            ${CODA_INSTALL_OPTION}
            LIBRARY DESTINATION "share/${ARG_MODULE_NAME}/plugins"
            ARCHIVE DESTINATION "share/${ARG_MODULE_NAME}/plugins"
            RUNTIME DESTINATION "share/${ARG_MODULE_NAME}/plugins")

    # install headers
    install(DIRECTORY "${CODA_STD_PROJECT_INCLUDE_DIR}/"
            DESTINATION "${CODA_STD_PROJECT_INCLUDE_DIR}/"
            FILES_MATCHING
                PATTERN "*.h"
                PATTERN "*.hpp"
            ${CODA_INSTALL_OPTION})

    # install conf directory, if present
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/conf")
        install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/conf"
                DESTINATION "share/${ARG_MODULE_NAME}"
                ${CODA_INSTALL_OPTION})
    endif()
endfunction()


# Add a SWIG Python module to the build
#
# Single value arguments:
#   TARGET          - Name of the CMake target to build the Python module
#   MODULE_NAME     - Name of the module within Python
#   INPUT           - Source file (.i) from which to generate the SWIG bindings
#   PACKAGE         - Name of the package to which this module is added
#
# Multi value arguments:
#   MODULE_DEPS     - List of compiled module dependencies for the library
#   PYTHON_DEPS     - List of Python module dependencies for the library
#
function(coda_add_swig_python_module)
    cmake_parse_arguments(
        "ARG"                               # prefix
        ""                                  # options
        "TARGET;MODULE_NAME;INPUT;PACKAGE"  # single args
        "MODULE_DEPS;PYTHON_DEPS"           # multi args
        "${ARGN}"
    )
    if (NOT ARG_PACKAGE)
        message(FATAL_ERROR "package must be specified for Python module ${ARG_TARGET}")
    endif()

    set(swig_include_dirs
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source>"
        "$<BUILD_INTERFACE:${CODA-OSS_SWIG_INCLUDE_DIR}>" # this one is for downstream projects
    )
    # determine the necessary SWIG include directories from the Python dependencies
    foreach(dep ${ARG_PYTHON_DEPS})
        get_property(dep_swig_include_dirs TARGET ${dep}
                     PROPERTY SWIG_INCLUDE_DIRECTORIES)
        list(APPEND swig_include_dirs ${dep_swig_include_dirs})
    endforeach()

    set_property(SOURCE ${ARG_INPUT} PROPERTY USE_TARGET_INCLUDE_DIRECTORIES ON)
    set_property(SOURCE ${ARG_INPUT} PROPERTY CPLUSPLUS ON)
    set_property(SOURCE ${ARG_INPUT} PROPERTY SWIG_MODULE_NAME ${ARG_MODULE_NAME})
    # where the generated Python files are written
    set(CMAKE_SWIG_OUTDIR "${CMAKE_CURRENT_SOURCE_DIR}/source/generated")
    # where the generated C/C++ files are written
    set(SWIG_OUTFILE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source/generated")

    if (SWIG_FOUND)
        swig_add_library(${ARG_TARGET} LANGUAGE python SOURCES ${ARG_INPUT})
        set_property(TARGET ${ARG_TARGET} PROPERTY
            SWIG_INCLUDE_DIRECTORIES ${swig_include_dirs})
        set_property(TARGET ${ARG_TARGET} PROPERTY
            SWIG_GENERATED_INCLUDE_DIRECTORIES ${Python_INCLUDE_DIRS})
    else()
        # use saved SWIG outputs in repo

        # this is the naming convention for waf, use for now since the
        # waf generated sources are committed to the repo
        set(source "source/generated/${ARG_MODULE_NAME}_wrap.cxx")
        if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${source}")
            # this is the naming convention for CMake
            # maybe switch this to the default in the future
            string(REPLACE "-python" "" source ${ARG_TARGET})
            string(REPLACE "." "_" source ${source})
            set(source "source/generated/${source}PYTHON_wrap.cxx")
        endif()
        message("using pre-generated SWIG source for ${ARG_TARGET}: ${source}")
        add_library(${ARG_TARGET} MODULE ${source})
        target_include_directories(${ARG_TARGET} PRIVATE ${Python_INCLUDE_DIRS})

        # extra settings from UseSWIG.cmake
        set_target_properties(${ARG_TARGET} PROPERTIES NO_SONAME ON)
        set_target_properties(${ARG_TARGET} PROPERTIES PREFIX "_")
        if(WIN32 AND NOT CYGWIN)
            set_target_properties(${ARG_TARGET} PROPERTIES SUFFIX ".pyd")
        endif()
    endif()

    target_link_libraries(${ARG_TARGET} PRIVATE
        ${ARG_MODULE_DEPS}
        ${Python_LIBRARIES})
    set_property(TARGET ${ARG_TARGET} PROPERTY
        LIBRARY_OUTPUT_NAME ${ARG_MODULE_NAME})

    # install the compiled extension library
    install(TARGETS ${ARG_TARGET}
            EXPORT ${CODA_EXPORT_SET_NAME}
            DESTINATION "${CODA_PYTHON_SITE_PACKAGES}/${ARG_PACKAGE}"
            ${CODA_INSTALL_OPTION})

    # install the python scripts
    file(GLOB sources_py
        "${CMAKE_CURRENT_SOURCE_DIR}/source/*.py"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/generated/*.py")
    install(FILES ${sources_py}
            DESTINATION "${CODA_PYTHON_SITE_PACKAGES}/${ARG_PACKAGE}"
            ${CODA_INSTALL_OPTION})

    # install the swig interface file, may be needed by downstream projects
    install(FILES ${ARG_INPUT}
            DESTINATION "${CODA_STD_PROJECT_INCLUDE_DIR}/swig"
            ${CODA_INSTALL_OPTION})
endfunction()

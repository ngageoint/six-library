set(TARGET_NAME pcre2)

if (PCRE_HOME)
    find_library(pcre2_PATH
                 pcre2-8
                 PATHS ${PCRE_HOME}
                 PATH_SUFFIXES lib lib64
                 NO_DEFAULT_PATH)
    if ("${pcre2_PATH}" STREQUAL "pcre2_PATH-NOTFOUND")
        message(FATAL_ERROR "pcre2 not found at PCRE_HOME ${PCRE_HOME}")
    else()
        message("Found pcre2 at ${pcre2_PATH}")
    endif()

    # verify we can link pcre
    set(CMAKE_REQUIRED_INCLUDES "${PCRE_HOME}/include")
    set(CMAKE_REQUIRED_LIBRARIES ${pcre2_PATH})
    set(CMAKE_REQUIRED_DEFINITIONS -DPCRE2_CODE_UNIT_WIDTH=8)
    unset(PCRE_HOME_VALID CACHE)
    check_cxx_source_compiles("
        #include <pcre2.h>
        int main() { pcre2_code* p; return 0; };"
        PCRE_HOME_VALID)
    unset(CMAKE_REQUIRED_DEFINITIONS)
    unset(CMAKE_REQUIRED_LIBRARIES)
    unset(CMAKE_REQUIRED_INCLUDES)
    if (NOT PCRE_HOME_VALID)
        message(FATAL_ERROR "Could not link with pcre2 from PCRE_HOME ${PCRE_HOME}")
    endif()

    # create interface library pointing to the external pcre2
    add_library(${TARGET_NAME} INTERFACE)
    target_include_directories(${TARGET_NAME} INTERFACE "${PCRE_HOME}/include")
    target_link_libraries(${TARGET_NAME} INTERFACE ${pcre2_PATH})
    target_compile_definitions(${TARGET_NAME} INTERFACE -DPCRE2_CODE_UNIT_WIDTH=8)
else()
    message("PCRE_HOME not specified, so it will be built")

    coda_fetch_driver(
        NAME ${TARGET_NAME}
        ARCHIVE "pcre2-10.22.tar.gz"
        HASH "SHA256=6677C66014EBA5610B8FBD8196FD4E5904DB4116CFF1B5A0D14E91121EDA4377"
    )

    check_symbol_exists("bcopy" "strings.h" HAVE_BCOPY)
    check_include_file("inttypes.h" HAVE_INTTYPES_H)
    check_symbol_exists("memmove" "string.h" HAVE_MEMMOVE)
    check_include_file("stdint.h" HAVE_STDINT_H)
    check_symbol_exists("strerror" "string.h" HAVE_STRERROR)
    set(SUPPORT_PCRE2_8 1)
    set(PCRE2_LINK_SIZE 2)
    set(PCRE2_MATCH_LIMIT 10000000)
    set(PCRE2_MATCH_LIMIT_RECURSION ${PCRE2_MATCH_LIMIT})
    set(MAX_NAME_COUNT 10000)
    set(MAX_NAME_SIZE 32)
    # This is "\n"
    set(NEWLINE_DEFAULT 2)
    set(PCRE2_PARENS_NEST_LIMIT 250)
    if (NOT BUILD_SHARED_LIBS)
        set(PCRE2_STATIC 1)
    endif()

    # Here are some other things in config.h that aren't used either
    # seemingly at all or only by things like pcregrep.c so not
    # wasting time doing config checks on them
    # STDC_HEADERS
    # HAVE_VISIBILITY
    # HAVE_PTHREAD
    # HAVE_PTHREAD_PRIO_INHERIT
    # PACKAGE*
    # conf.check_cc(header_name='bzlib.h', mandatory=False)
    # conf.check_cc(header_name='dirent.h', mandatory=False)
    # conf.check_cc(header_name='dlfcn.h', mandatory=False)
    # conf.check_cc(header_name='editline/readline.h', mandatory=False)
    # conf.check_cc(header_name='edit/readline/readline.h', mandatory=False)

    # conf.check_cc(header_name='stdlib.h', mandatory=False)
    # conf.check_cc(header_name='memory.h', mandatory=False)
    # conf.check_cc(header_name='limits.h', mandatory=False)
    # conf.check_cc(header_name='readline/history.h', mandatory=False)
    # conf.check_cc(header_name='readline/readline.h', mandatory=False)
    # conf.check_cc(header_name='sys/stat.h', mandatory=False)
    # conf.check_cc(header_name='sys/types.h', mandatory=False)
    # conf.check_cc(header_name='sys/wait.h', mandatory=False)
    # conf.check_cc(header_name='unistd.h', mandatory=False)
    # conf.check_cc(header_name='strings.h', mandatory=False)
    # conf.check_cc(header_name='string.h', mandatory=False)
    # conf.check_cc(header_name='windows.h', mandatory=False)
    # conf.check_cc(header_name='zlib.h', mandatory=False)

    set(SOURCE_DIR "${${CMAKE_PROJECT_NAME}_${TARGET_NAME}_SOURCE_DIR}")

    # make config.h
    configure_file("${SOURCE_DIR}/config-cmake.h.in"
                   "config.h")

    # Slide pcre2.h.generic into place as pcre2.h
    configure_file("${SOURCE_DIR}/src/pcre2.h.generic"
                   "pcre2.h"
                   COPYONLY)

    # Slide pcre2_chartables.c.dist into place as pcre2_chartables.c
    # NOTE: Older versions of PCRE used to be more complicated because
    # we used to need to compile a dftables executable in the build
    # step to generate this (look at older versions of this wscript if
    # this ever needs to be resurrected).  With the normal PCRE2
    # configure, you can do this via --enable-rebuild-chartables, but
    # what it generates is identical to this file
    configure_file("${SOURCE_DIR}/src/pcre2_chartables.c.dist"
                    "pcre2_chartables.c"
                    COPYONLY)

    set(SOURCES "${CMAKE_CURRENT_BINARY_DIR}/pcre2_chartables.c")
    foreach(src "pcre2_auto_possess.c"
                "pcre2_compile.c"
                "pcre2_config.c"
                "pcre2_context.c"
                "pcre2_dfa_match.c"
                "pcre2_error.c"
                "pcre2_find_bracket.c"
                "pcre2_jit_compile.c"
                "pcre2_maketables.c"
                "pcre2_match.c"
                "pcre2_match_data.c"
                "pcre2_newline.c"
                "pcre2_ord2utf.c"
                "pcre2_pattern_info.c"
                "pcre2_serialize.c"
                "pcre2_string_utils.c"
                "pcre2_study.c"
                "pcre2_substitute.c"
                "pcre2_substring.c"
                "pcre2_tables.c"
                "pcre2_ucd.c"
                "pcre2_valid_utf.c"
                "pcre2_xclass.c")
        list(APPEND SOURCES "${SOURCE_DIR}/src/${src}")
    endforeach()

    add_library(${TARGET_NAME} ${SOURCES})
    set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME pcre2-8)
    target_compile_definitions(${TARGET_NAME} PUBLIC PCRE2_CODE_UNIT_WIDTH=8
                                                PRIVATE HAVE_CONFIG_H)
    target_include_directories(${TARGET_NAME}
        PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
               "$<BUILD_INTERFACE:${SOURCE_DIR}/src>"
               "$<INSTALL_INTERFACE:${CODA_STD_PROJECT_INCLUDE_DIR}>")
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/pcre2.h"
            DESTINATION "${CODA_STD_PROJECT_INCLUDE_DIR}"
            ${CODA_INSTALL_OPTION})
endif()
install(TARGETS ${TARGET_NAME}
        EXPORT ${CODA_EXPORT_SET_NAME}
        ${CODA_INSTALL_OPTION}
        LIBRARY DESTINATION "${CODA_STD_PROJECT_LIB_DIR}"
        ARCHIVE DESTINATION "${CODA_STD_PROJECT_LIB_DIR}")

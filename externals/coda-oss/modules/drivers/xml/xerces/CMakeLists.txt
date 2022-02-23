set(TARGET_NAME xerces-c)

if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    # use this CMakeLists.txt to build and package zlib with conan
    project(coda-oss_xerces-c)
    include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    conan_basic_setup()
    set(CODA_SKIP_SYSTEM_DEPENDENCIES ON)
    include(CodaBuild.cmake)
    coda_initialize_build()
    coda_generate_package_config()
endif()

if (WIN32)
    set(EXTERNAL_DEPS ws2_32 advapi32)
elseif (UNIX)
    set(EXTERNAL_DEPS pthread)
endif()

if (XML_HOME)
    find_library(${TARGET_NAME}_PATH
                 ${TARGET_NAME}
                 PATHS ${XML_HOME}
                 PATH_SUFFIXES lib lib64
                 NO_DEFAULT_PATH)
    if ("${${TARGET_NAME}_PATH}" STREQUAL "${TARGET_NAME}_PATH-NOTFOUND")
        message(FATAL_ERROR "Xerces not found at XML_HOME ${XML_HOME}")
    else()
        message("Found Xerces at ${${TARGET_NAME}_PATH}")
    endif()

    # verify we can link this library
    string(REGEX MATCH ".*\.(a|lib)$" XERCES_IS_STATIC ${${TARGET_NAME}_PATH})
    if (XERCES_IS_STATIC)
        set(CMAKE_REQUIRED_DEFINITIONS -DXERCES_STATIC_LIBRARY)
    endif()
    set(CMAKE_REQUIRED_INCLUDES "${XML_HOME}/include")
    set(CMAKE_REQUIRED_LIBRARIES ${${TARGET_NAME}_PATH} ${EXTERNAL_DEPS})
    unset(XERCES_HOME_VALID CACHE)
    check_cxx_source_compiles("
        #include <xercesc/util/PlatformUtils.hpp>
        int main() { xercesc::XMLPlatformUtils::Terminate(); return 0; }"
        XERCES_HOME_VALID)
    unset(CMAKE_REQUIRED_LIBRARIES)
    unset(CMAKE_REQUIRED_INCLUDES)
    unset(CMAKE_REQUIRED_DEFINITIONS)
    if (NOT XERCES_HOME_VALID)
        message(FATAL_ERROR "Could not link with Xerces from XML_HOME ${XML_HOME}")
    endif()

    # create interface library pointing to the external Xerces
    add_library(${TARGET_NAME} INTERFACE)
    target_include_directories(${TARGET_NAME} INTERFACE "${XML_HOME}/include")
    target_link_libraries(${TARGET_NAME} INTERFACE ${${TARGET_NAME}_PATH} ${EXTERNAL_DEPS})
    if (XERCES_IS_STATIC)
        target_compile_definitions(${TARGET_NAME} INTERFACE XERCES_STATIC_LIBRARY)
    endif()
else()
    message("XML_HOME not specified, so XercesC will be built")
    coda_fetch_driver(
        NAME ${TARGET_NAME}
        ARCHIVE "xerces-c-3.2.3.tar"
        HASH "SHA256=61981c7a464e33913a093e0dfe7daf8a45f21e46b1924eb97f92bdbb746e2e6e"
    )

    check_include_file("inttypes.h" HAVE_INTTYPES_H)
    check_include_file("netinet/in.h" HAVE_NETINET_IN_H)
    check_include_file("arpa/inet.h" HAVE_ARPA_INET_H)
    check_include_file("netdb.h" HAVE_NETDB_H)
    check_include_file("intrin.h" HAVE_INTRIN_H)
    check_include_file("emmintrin.h" HAVE_EMMINTRIN_H)
    check_include_file("wchar.h" HAVE_WCHAR_H)
    check_include_file("stdint.h" HAVE_STDINT_H)
    check_include_file("stdio.h" HAVE_STDIO_H)
    check_include_file("langinfo.h" HAVE_LANGINFO_H)
    check_include_file("iconv.h" HAVE_ICONV_H)
    check_include_file("nl_types.h" HAVE_NL_TYPES_H)
    check_include_file("ctype.h" HAVE_CTYPE_H)
    check_include_file("wctype.h" HAVE_WCTYPE_H)
    check_include_file("cpuid.h" HAVE_CPUID_H)
    check_include_file("endian.h" HAVE_ENDIAN_H)
    check_include_file("machine/endian.h" HAVE_MACHINE_ENDIAN_H)
    check_include_file("arpa/nameser_compat.h" HAVE_ARPA_NAMESER_COMPAT_H)
    check_include_file("errno.h" HAVE_ERRNO_H)
    check_include_file("float.h" HAVE_FLOAT_H)
    check_include_file("locale.h" HAVE_LOCALE_H)
    check_include_file("sys/param.h" HAVE_SYS_PARAM_H)
    check_include_file("sys/socket.h" HAVE_SYS_SOCKET_H)
    check_include_file("sys/stat.h" HAVE_SYS_STAT_H)
    check_include_file("sys/timeb.h" HAVE_SYS_TIMEB_H)
    check_include_file("sys/types.h" HAVE_SYS_TYPES_H)

    check_symbol_exists("mblen" "stdlib.h" HAVE_MBLEN)
    check_symbol_exists("mbrlen" "stdlib.h" HAVE_MBRLEN)
    check_symbol_exists("mbsrtowcs" "stdlib.h" HAVE_MBSRTOWCS)
    check_symbol_exists("mbstowcs" "stdlib.h" HAVE_MBSTOWCS)
    check_symbol_exists("wsctombs" "stdlib.h" HAVE_WCSTOMBS)
    check_symbol_exists("wcsrtombs" "stdlib.h" HAVE_WCSRTOMBS)
    check_symbol_exists("realpath" "stdlib.h" HAVE_REALPATH)
    check_symbol_exists("strchr" "string.h" HAVE_STRCHR)
    check_symbol_exists("strdup" "string.h" HAVE_STRDUP)
    check_symbol_exists("stricmp" "string.h" HAVE_STRICMP)
    check_symbol_exists("strnicmp" "string.h" HAVE_STRNICMP)
    check_symbol_exists("strrcmp" "string.h" HAVE_STRRCMP)
    check_symbol_exists("strstr" "string.h" HAVE_STRSTR)
    check_symbol_exists("strtol" "string.h" HAVE_STRTOL)
    check_symbol_exists("strtoul" "string.h" HAVE_STRTOUL)
    check_symbol_exists("strcasecmp" "strings.h" HAVE_STRCASECMP)
    check_symbol_exists("strncasecmp" "strings.h" HAVE_STRNCASECMP)
    check_symbol_exists("pathconf" "unistd.h" HAVE_PATHCONF)
    check_symbol_exists("nl_langinfo" "langinfo.h" HAVE_NL_LANGINFO)
    check_symbol_exists("iconv" "iconv.h" HAVE_ICONV)
    check_symbol_exists("iconv_close" "iconv.h" HAVE_ICONV_CLOSE)
    check_symbol_exists("iconv_open" "iconv.h" HAVE_ICONV_OPEN)
    check_symbol_exists("towupper" "wctype.h" HAVE_TOWUPPER)
    check_symbol_exists("towlower" "wctype.h" HAVE_TOWLOWER)
    check_symbol_exists("catclose" "nl_types.h" HAVE_CATCLOSE)
    check_symbol_exists("catgets" "nl_types.h" HAVE_CATGETS)
    check_symbol_exists("catopen" "nl_types.h" HAVE_CATOPEN)
    check_symbol_exists("localeconv" "locale.h" HAVE_LOCALECONV)
    check_symbol_exists("setlocale" "locale.h" HAVE_SETLOCALE)
    check_symbol_exists("socket" "sys/socket.h" HAVE_SOCKET)
    check_symbol_exists("ftime" "sys/timeb.h" HAVE_FTIME)
    check_symbol_exists("getaddrinfo" "netdb.h" HAVE_GETADDRINFO)

    find_library(NSL_LIB nsl)
    if (NOT NSL_LIB STREQUAL NSL_LIB-NOTFOUND)
        set(HAVE_NSL 1)
    endif()

    check_include_file("unistd.h" HAVE_UNISTD_H)
    check_include_file("sys/time.h" HAVE_SYS_TIME_H)
    check_include_file("limits.h" HAVE_LIMITS_H)
    check_include_file("strings.h" HAVE_STRINGS_H)

    # extra defines for config.h
    if (HAVE_ICONV_H)
        set(ICONV_USES_CONST_POINTER 0)
    endif()
    if (HAVE_SYS_TIME_H AND HAVE_TIME_H)
        set(TIME_WITH_SYS_TIME 1)
    endif()
    if (HAVE_SYS_TYPES_H)
        set(XERCES_HAVE_SYS_TYPES_H 1)
    endif()
    if (HAVE_INTTYPES_H)
        set(XERCES_HAVE_INTTYPES_H 1)
    endif()
    if (HAVE_INTRIN_H)
        set(XERCES_HAVE_INTRIN_H 1)
    endif()
    if (HAVE_EMMINTRIN_H)
        set(XERCES_HAVE_EMMINTRIN_H 1)
    endif()
    if (HAVE_WCHAR_H)
        set(XERCES_INCLUDE_WCHAR_H 1)
    endif()
    if (HAVE_EMMINTRIN_H)
        set(XERCES_HAVE_SSE2_INTRINSIC 1)
    endif()
    if (WIN32)
        set(XERCES_USE_FILEMGR_WINDOWS 1)
        set(XERCES_USE_MUTEXMGR_WINDOWS 1)
        set(XERCES_USE_NETACCESSOR_WINSOCK 1)
        set(XERCES_USE_TRANSCODER_WINDOWS 1)
        set(XERCES_PATH_DELIMITER_BACKSLASH 1)

    # -- please note -- we diverge from the xerces-c make
    # system and treat all unix platforms similarly. Solaris
    # was make configured to use ICU and Linux the GNUICONV,
    # but here we use the regular ICONV for both to avoid
    # having complicated checks and additional build logic.
    else()
        set(HAVE_PTHREAD 1)

        set(XERCES_USE_FILEMGR_POSIX 1)
        set(XERCES_USE_MUTEXMGR_POSIX 1)
        set(XERCES_USE_TRANSCODER_ICONV 1)
    endif()

    set(XERCES_S16BIT_INT "int16_t")
    set(XERCES_S32BIT_INT "int32_t")
    set(XERCES_S64BIT_INT "int64_t")
    set(XERCES_U16BIT_INT "uint16_t")
    set(XERCES_U32BIT_INT "uint32_t")
    set(XERCES_U64BIT_INT "uint64_t")
    set(XERCES_SIZE_T "size_t")
    # This is for a UTF-16 value, 16 bits
    if (WIN32)
        set(XERCES_XMLCH_T "wchar_t")
    else()
        set(XERCES_XMLCH_T "char16_t")
    endif()
    if (HAVE_SSIZE_T)
        set(XERCES_SSIZE_T "ssize_t")
    else()
        if (SIZEOF_SIZE_T STREQUAL 8)
            set(XERCES_SSIZE_T "int64_t")
        else()
            set(XERCES_SSIZE_T "int32_t")
        endif()
    endif()
    set(XERCES_SIZE_MAX "SIZE_MAX")

    set(LT_OBJDIR .libs/)
    set(PACKAGE xerces-c)
    set(PACKAGE_BUGREPORT "")
    set(PACKAGE_NAME "xerces-c")
    set(PACKAGE_STRING "xerces-c 3.2.3")
    set(PACKAGE_TARNAME "xerces-c")
    set(PACKAGE_URL "")
    set(PACKAGE_VERSION "3.2.3")
    set(VERSION "3.2.3")

    # extra defines for Xerces_autoconf_config.hpp
    set(XERCES_USE_MSGLOADER_INMEMORY 1)
    set(XERCES_BUILDING_LIBRARY 1)
    set(HAVE_NAMESPACES 1)
    set(HAVE_SOCKET 1)
    set(HAVE_STD_LIBS 1)
    set(HAVE_STD_NAMESPACE 1)

    set(XERCES_AUTOCONF 1)
    set(XERCES_PLATFORM_EXPORT "")
    if (MSVC AND BUILD_SHARED_LIBS)
        set(XERCES_PLATFORM_EXPORT "__declspec(dllexport)")
    endif()
    set(XERCES_PLATFORM_IMPORT "")
    set(XERCES_TEMPLATE_EXTERN "extern")
    set(XERCES_HAS_CPP_NAMESPACE 1)
    set(XERCES_STD_NAMESPACE 1)
    set(XERCES_NEW_IOSTREAMS 1)
    set(XERCES_LSTRSUPPORT 1)
    set(HAVE_BOOL 1)

    if (HAVE_NL_TYPES_H)
        set(HAVE_CATCLOSE 1)
        set(HAVE_CATGETS 1)
        set(HAVE_CATOPEN 1)
    endif()

    set(SOURCE_DIR "${${CMAKE_PROJECT_NAME}_${TARGET_NAME}_SOURCE_DIR}")
    configure_file("config.h.in" "config.h")
    configure_file("Xerces_autoconf_config.hpp.in" "xercesc/util/Xerces_autoconf_config.hpp")
    # setup some known excludes per platform --
    # we avoid building the system specific files
    # and instead build the safely cross-platform files for all unix
    # platforms. -- for more information see note above --
    set(EXCLUDES "URLAccessCFBinInputStream.cpp"
                 "MacOSURLAccessCF.cpp" "IconvGNUTransService.cpp"
                 "MacOSUnicodeConverter.cpp" "MsgCatalogLoader.cpp"
                 "CurlURLInputStream.cpp" "CurlNetAccessor.cpp"
                 "ICUMsgLoader.cpp" "ICUTransService.cpp"
                 "Win32MsgLoader.cpp")
    set(UNIX_SPECIFIC "SocketNetAccessor.cpp" "UnixHTTPURLInputStream.cpp"
                      "PosixFileMgr.cpp" "PosixMutexMgr.cpp"
                      "IconvTransService.cpp")
    set(WIN_SPECIFIC "Win32TransService.cpp" "WinSockNetAccessor.cpp"
                     "BinHTTPURLInputStream.cpp"
                     "WindowsFileMgr.cpp" "WindowsMutexMgr.cpp")

    # create filter for system specific files
    # MacOS could be supported here as well
    if (WIN32)
        list(APPEND EXCLUDES ${UNIX_SPECIFIC})
    else()
        list(APPEND EXCLUDES ${WIN_SPECIFIC})
    endif()

    file(GLOB_RECURSE SOURCES "${SOURCE_DIR}/src/*.cpp")
    list(FILTER SOURCES EXCLUDE REGEX ".*Test.*")
    foreach(exclude_file ${EXCLUDES})
        list(FILTER SOURCES EXCLUDE REGEX ".*${exclude_file}.*")
    endforeach()

    if (NOT WIN32)
        file(GLOB C_SOURCES "${SOURCE_DIR}/src/*.c")
        list(APPEND SOURCES ${C_SOURCES})
    endif()

    add_library(${TARGET_NAME} ${SOURCES})
    target_link_libraries(${TARGET_NAME} PUBLIC ${EXTERNAL_DEPS})

    set(COMPILE_DEFS_PRIVATE HAVE_CONFIG_H XERCES_BUILDING_LIBRARY)
    if (BUILD_SHARED)
        list(APPEND COMPILE_DEFS_PRIVATE DLL_EXPORT)
    else()
        list(APPEND COMPILE_DEFS_PUBLIC XERCES_STATIC_LIBRARY)
    endif()
    target_compile_definitions(${TARGET_NAME}
        PUBLIC ${COMPILE_DEFS_PUBLIC}
        PRIVATE ${COMPILE_DEFS_PRIVATE})
    target_include_directories(${TARGET_NAME}
        PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
               "$<BUILD_INTERFACE:${SOURCE_DIR}/src>"
               "$<INSTALL_INTERFACE:${CODA_STD_PROJECT_INCLUDE_DIR}>")
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/xercesc/util/Xerces_autoconf_config.hpp"
            DESTINATION "${CODA_STD_PROJECT_INCLUDE_DIR}/xercesc/util"
            ${CODA_INSTALL_OPTION})
    install(DIRECTORY "${SOURCE_DIR}/src/xercesc"
            DESTINATION "${CODA_STD_PROJECT_INCLUDE_DIR}"
            ${CODA_INSTALL_OPTION}
            FILES_MATCHING
                PATTERN "*.h"
                PATTERN "*.hpp"
                PATTERN "*.c")
endif()
install(TARGETS ${TARGET_NAME}
        EXPORT ${CODA_EXPORT_SET_NAME}
        ${CODA_INSTALL_OPTION}
        LIBRARY DESTINATION "${CODA_STD_PROJECT_LIB_DIR}"
        ARCHIVE DESTINATION "${CODA_STD_PROJECT_LIB_DIR}")

set(XML_LIB xerces-c CACHE INTERNAL "library providing XML support")

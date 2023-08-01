set(TARGET_NAME uuid)

if (WIN32)
    # for Windows we use the UuidCreateSequential function, which requires
    # linking with the system library rpcrt4
    set(UUID_LIB rpcrt4 CACHE INTERNAL "library for providing UUID")
else()
    set(UUID_HOME "" CACHE PATH "path to UUID installation")
    if (UUID_HOME)
        find_library(${TARGET_NAME}_PATH
                     ${TARGET_NAME}
                     PATHS ${UUID_HOME}
                     PATH_SUFFIXES lib lib64
                     NO_DEFAULT_PATH)
        if ("${${TARGET_NAME}_PATH}" STREQUAL "${TARGET_NAME}_PATH-NOTFOUND")
            message(FATAL_ERROR "${TARGET_NAME} not found at UUID_HOME ${UUID_HOME}")
        else()
            message("Found ${TARGET_NAME} at ${${TARGET_NAME}_PATH}")
        endif()

        # verify we can link the found library
        set(CMAKE_REQUIRED_INCLUDES "${UUID_HOME}/include")
        set(CMAKE_REQUIRED_LIBRARIES ${${TARGET_NAME}_PATH})
        unset(UUID_HOME_VALID CACHE)
        check_cxx_source_compiles("
            #include <uuid/uuid.h>
            int main() { auto f = &uuid_generate; return 0; };"
            UUID_HOME_VALID)
        unset(CMAKE_REQUIRED_LIBRARIES)
        unset(CMAKE_REQUIRED_INCLUDES)
        if (NOT UUID_HOME_VALID)
            message(FATAL_ERROR "Could not link with UUID from UUID_HOME ${UUID_HOME}")
        endif()

        # create interface library pointing to the external library
        add_library(${TARGET_NAME} INTERFACE)
        target_include_directories(${TARGET_NAME} INTERFACE "${UUID_HOME}/include")
        target_link_libraries(${TARGET_NAME} INTERFACE ${uuid_PATH})
    else()
        message("UUID_HOME not specified, so it will be built")

        coda_fetch_driver(
            NAME ${TARGET_NAME}
            ARCHIVE "e2fsprogs-1.40-uuid.tar"
            HASH "SHA256=3507c7e9f226656b3903651c038e9c4907a31282cc4ba86aed8a588e6cc4cec2"
        )

        set(SOURCE_DIR "${${CMAKE_PROJECT_NAME}_${TARGET_NAME}_SOURCE_DIR}")

        set(HAVE_INTTYPES_H true)
        check_type_size("int" SIZEOF_INT)
        check_type_size("short" SIZEOF_SHORT)
        check_type_size("long" SIZEOF_LONG)
        check_type_size("long long" SIZEOF_LONG_LONG)
        configure_file("${SOURCE_DIR}/lib/uuid/uuid_types.h.in"
                       "uuid/uuid_types.h"
                       @ONLY)

        foreach(src "clear.c" "compare.c" "copy.c" "gen_uuid.c" "isnull.c"
                    "pack.c" "parse.c" "unpack.c" "unparse.c" "uuid_time.c")
            list(APPEND SOURCES "${SOURCE_DIR}/lib/uuid/${src}")
        endforeach()

        add_library(${TARGET_NAME} ${SOURCES})
        target_compile_definitions(${TARGET_NAME} PRIVATE
            HAVE_INTTYPES_H=${HAVE_INTTYPES_H})
        target_include_directories(${TARGET_NAME}
            PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
                   "$<BUILD_INTERFACE:${SOURCE_DIR}/lib>"
                   "$<INSTALL_INTERFACE:${CODA_STD_PROJECT_INCLUDE_DIR}>")

        install(FILES "${SOURCE_DIR}/lib/uuid/uuid.h"
                DESTINATION "${CODA_STD_PROJECT_INCLUDE_DIR}/uuid"
                ${CODA_INSTALL_OPTION})
    endif()

    install(TARGETS ${TARGET_NAME}
            EXPORT ${CODA_EXPORT_SET_NAME}
            ${CODA_INSTALL_OPTION}
            LIBRARY DESTINATION "${CODA_STD_PROJECT_LIB_DIR}"
            ARCHIVE DESTINATION "${CODA_STD_PROJECT_LIB_DIR}")

    set(UUID_LIB ${TARGET_NAME} CACHE INTERNAL "library for providing UUID")
endif()

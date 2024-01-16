set(TARGET_NAME uuid)

if (WIN32)
    # for Windows we use the UuidCreateSequential function, which requires
    # linking with the system library rpcrt4
    set(UUID_LIB rpcrt4 CACHE INTERNAL "library for providing UUID")
else()
        message("UUID_HOME not specified, so it will be built")

        coda_fetch_driver(
            NAME ${TARGET_NAME}
            ARCHIVE "e2fsprogs-1.47.0.tar"
            HASH "SHA256=c7c3a26bee8bb3b0041ee8701a5f47a29e81ee0b20d5995bc4544638cd0ea394"
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

    install(TARGETS ${TARGET_NAME}
            EXPORT ${CODA_EXPORT_SET_NAME}
            ${CODA_INSTALL_OPTION}
            LIBRARY DESTINATION "${CODA_STD_PROJECT_LIB_DIR}"
            ARCHIVE DESTINATION "${CODA_STD_PROJECT_LIB_DIR}")

    set(UUID_LIB ${TARGET_NAME} CACHE INTERNAL "library for providing UUID")
endif()

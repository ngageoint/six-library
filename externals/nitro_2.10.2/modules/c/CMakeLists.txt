set(NITRO_VERSION "2.9")
set(TARGET_LANGUAGE c)

add_subdirectory(nrt)
add_subdirectory(nitf)
add_subdirectory(cgm)

if (ENABLE_J2K)
    add_subdirectory(j2k)
endif()

add_subdirectory(jpeg)

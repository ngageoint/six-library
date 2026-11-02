# disable some unneeded coda-oss components
set(ENABLE_JARS OFF CACHE BOOL "" FORCE)
add_subdirectory("${CODA_OSS_DIR}")

# six requires some static TREs from nitro, so we force enable them
set(ENABLE_STATIC_TRES ON CACHE BOOL "Build static libraries for each TRE" FORCE)
set(ENABLE_JAVA OFF CACHE BOOL "" FORCE)
set(ENABLE_MEX OFF CACHE BOOL "" FORCE)

set(NITRO_DIR "nitro")
add_subdirectory("${NITRO_DIR}")

if (MSVC)
    add_compile_options(/W0)
endif()

# FetchContent_Declare(mdspan
#     URL "${CMAKE_CURRENT_SOURCE_DIR}/mdspan-mdspan-0.6.0.tar.gz"
#     URL_HASH "SHA256=79f94d7f692cbabfbaff6cd0d3434704435c853ee5087b182965fa929a48a592")
# FetchContent_MakeAvailable(mdspan)
FetchContent_Declare(mdspan
    URL "${CMAKE_CURRENT_SOURCE_DIR}/mdspan-develop-a32a23b5.zip"
    URL_HASH "SHA256=8546d5a63047f79260ecdef0ee331a070352fd34b8d175d0392007f4b1d887bc")
FetchContent_MakeAvailable(mdspan)


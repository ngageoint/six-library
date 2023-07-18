# disable some unneeded coda-oss components
set(ENABLE_JARS OFF CACHE BOOL "" FORCE)
add_subdirectory("${CODA_OSS_DIR}")

# six requires some static TREs from nitro, so we force enable them
set(ENABLE_STATIC_TRES ON CACHE BOOL "Build static libraries for each TRE" FORCE)
set(ENABLE_JAVA OFF CACHE BOOL "" FORCE)
set(ENABLE_MEX OFF CACHE BOOL "" FORCE)

set(NITRO_DIR "nitro")
add_subdirectory("${NITRO_DIR}")

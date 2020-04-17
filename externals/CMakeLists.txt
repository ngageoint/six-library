add_subdirectory("coda-oss")

# six requires some static TREs from nitro, so we force enable them
set(ENABLE_STATIC_TRES ON CACHE BOOL "Build static libraries for each TRE" FORCE)
add_subdirectory("nitro")

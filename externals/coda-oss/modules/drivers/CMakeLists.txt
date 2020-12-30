add_subdirectory("boost")
#add_subdirectory("curl") # this is handled in coda_find_system_dependencies
#add_subdirectory("numpy") # this is handled in coda_find_system_dependencies
add_subdirectory("xml")
#add_subdirectory("gsl") # nothing to do

set(ENABLE_J2K ON CACHE BOOL "enable J2K library")
set(J2K_HOME "" CACHE PATH "path to J2K installation")
if (ENABLE_J2K OR J2K_HOME)
    add_subdirectory("j2k")
endif()

set(ENABLE_JARS ON CACHE BOOL "include jars in the install")
if (ENABLE_JARS)
    add_subdirectory("jars")
endif()

set(ENABLE_JPEG ON CACHE BOOL "enable use of libjpeg")
set(JPEG_HOME "" CACHE PATH "path to libjpeg installation")
if (ENABLE_JPEG OR JPEG_HOME)
    add_subdirectory("jpeg")
endif()

set(ENABLE_PCRE ON CACHE BOOL "enable PCRE library")
set(PCRE_HOME "" CACHE PATH "path to PCRE installation")
if (ENABLE_PCRE OR PCRE_HOME)
    add_subdirectory("pcre")
endif()

set(SQL_LAYER "" CACHE STRING "SQL backend [mysql, psql, oracle]")
set(SQL_HOME "" CACHE PATH "Path to SQL installation")
if (SQL_LAYER OR SQL_HOME)
    add_subdirectory("sql")
endif()

set(ENABLE_UUID ON CACHE BOOL "enable UUID library")
if (ENABLE_UUID OR UUID_HOME)
    add_subdirectory("uuid")
endif()

set(ENABLE_ZIP ON CACHE BOOL "enable zlib")
set(ZIP_HOME "" CACHE PATH "path to pre-existing zlib installation, if not provided zlib will be built")
if (ENABLE_ZIP OR ZIP_HOME)
    add_subdirectory("zlib")
endif()

set(TARGET_LANGUAGE c++)

# turn on warnings as errors
if (MSVC)
    add_compile_options(/WX)
    add_compile_options(/W4)
elseif (UNIX)
    add_compile_options(-Werror)
endif()

# add an interface library for unittests
add_library(TestCase INTERFACE)
target_link_libraries(TestCase INTERFACE sys-c++ str-c++)
target_include_directories(TestCase INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:include>")
install(FILES "include/TestCase.h" DESTINATION "${CODA_STD_PROJECT_INCLUDE_DIR}")
install(TARGETS TestCase EXPORT ${CODA_EXPORT_SET_NAME})

# common configuration checks, used by config and sys modules
check_include_file("pthread.h" HAVE_PTHREAD_H)
check_include_file("execinfo.h" HAVE_EXECINFO_H)
check_symbol_exists("clock_gettime" "time.h" HAVE_CLOCK_GETTIME)
if (NOT HAVE_CLOCK_GETTIME) # On old systems this was in librt, not libc
    unset("HAVE_CLOCK_GETTIME" CACHE) # check_xxx_exists set CACHE variables, which cannot be re-used without being unset.
    find_library(RT_LIB rt)
    if (RT_LIB)
        check_library_exists(rt clock_gettime ${RT_LIB} HAVE_CLOCK_GETTIME)
        if (HAVE_CLOCK_GETTIME) # Record the necessary extra link library
            set(CLOCK_GETTIME_EXTRALIBS "rt" CACHE INTERNAL "")
        endif()
    endif()
endif()
check_include_file("atomic.h" HAVE_ATOMIC_H)
check_include_file("sys/time.h" HAVE_SYS_TIME_H)
check_symbol_exists("localtime_r" "time.h" HAVE_LOCALTIME_R)
check_symbol_exists("gmtime_r" "time.h" HAVE_GMTIME_R)
check_symbol_exists("setenv" "stdlib.h" HAVE_SETENV)
check_symbol_exists("posix_memalign" "stdlib.h" HAVE_POSIX_MEMALIGN)
check_symbol_exists("memalign" "stdlib.h" HAVE_MEMALIGN)
test_big_endian(BIGENDIAN)
check_type_size("size_t" SIZEOF_SIZE_T)
check_cxx_source_compiles(
    "int __attribute__((noinline)) fn() { return 0; } int main() { return fn(); }"
    HAVE_ATTRIBUTE_NOINLINE)
check_cxx_source_compiles(
    "int main() { int var __attribute__((aligned (32))); return var; }"
    HAVE_ATTRIBUTE_ALIGNED)

add_subdirectory("config")
add_subdirectory("coda_oss")
add_subdirectory("avx")
add_subdirectory("serialize")
add_subdirectory("except")
add_subdirectory("sys")
add_subdirectory("str")
add_subdirectory("mem")
add_subdirectory("re")
add_subdirectory("io")
add_subdirectory("zip")
add_subdirectory("sio.lite")
add_subdirectory("cli")
add_subdirectory("dbi")
add_subdirectory("types")
add_subdirectory("unique")
add_subdirectory("units")
add_subdirectory("math")
add_subdirectory("mt")
add_subdirectory("logging")
add_subdirectory("xml.lite")
add_subdirectory("net") # must be after "re"
add_subdirectory("net.ssl") # must be after "net"
add_subdirectory("plugin")
add_subdirectory("tiff")
add_subdirectory("polygon")
add_subdirectory("math.linear")
add_subdirectory("math.poly")
add_subdirectory("numpyutils")
add_subdirectory("gsl")
add_subdirectory("std")

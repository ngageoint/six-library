set(TARGET_LANGUAGE c++)
set(CMAKE_CXX_STANDARD 14)
set(CXX_STANDARD_REQUIRED true)

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
test_big_endian(BIGENDIAN)

add_subdirectory("config")
add_subdirectory("coda_oss")
add_subdirectory("avx")
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
add_subdirectory("hdf5.lite")
add_subdirectory("gsl")
add_subdirectory("std")


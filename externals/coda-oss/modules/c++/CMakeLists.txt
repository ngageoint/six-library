set(TARGET_LANGUAGE c++)

if (MSVC)
    # By default, there is a /W3 on the command-line from somewhere (?); adding
    # /Wn results in a compiler warning.
    #
    # https://github.com/microsoft/STL/wiki/Changelog#vs-2022-179-preview-1
    # > *Note*: `/Wall` is not intended for regular production use, as it contains a large number of
    # > extremely noisy and low-value warnings. In general, the STL does not attempt to be `/Wall` clean.
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}") # add_compile_options(/W4)

elseif (UNIX)
    # https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
    add_compile_options(-Werror) # Make all warnings into errors
    add_compile_options(-Wall -Wextra -Wpedantic -pedantic-errors -Wunused)

    add_compile_options(-Wzero-as-null-pointer-constant)
    add_compile_options(-Wsuggest-final-types -Wsuggest-final-methods)
    add_compile_options(-Wsuggest-override)
    add_compile_options(-Woverloaded-virtual)
    add_compile_options(-Warray-bounds)
    add_compile_options(-Wduplicated-branches -Wduplicated-cond)
    add_compile_options(-Wtrampolines)
    add_compile_options(-Wshadow)
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wold-style-cast")
    #add_compile_options(-Wfloat-equal)
    #add_compile_options(-Wconversion)
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weffc++")

    add_compile_options(-Wno-double-promotion) # implicit conversion of `float` to `double` is fine

    # Need a newer compiler than GCC 9
    #add_compile_options(-Wnrvo)
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


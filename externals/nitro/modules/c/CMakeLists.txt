set(NITRO_VERSION "2.9")
set(TARGET_LANGUAGE c)

# turn on warnings as errors
if (MSVC)
    # By default, there is a /W3 on the command-line from somewhere (?); adding
    # /Wn results in a compiler warning.
    #
    # https://github.com/microsoft/STL/wiki/Changelog#vs-2022-179-preview-1
    # > *Note*: `/Wall` is not intended for regular production use, as it contains a large number of
    # > extremely noisy and low-value warnings. In general, the STL does not attempt to be `/Wall` clean.
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}") # add_compile_options(/W4)

    add_compile_options(/wd4996) # '...': This function or variable may be unsafe.
elseif (UNIX)
    #add_compile_options(-Wall -pedantic -Wextra)
    add_compile_options(-Wall -Wextra)

    add_compile_options(-Wno-implicit-fallthrough)
    add_compile_options(-Wno-unused-function)
    add_compile_options(-Wno-sign-compare -Wno-pointer-sign)
    add_compile_options(-Wno-missing-field-initializers -Wno-maybe-uninitialized)
    add_compile_options(-Wno-unused-parameter)
    add_compile_options(-Wno-cast-function-type)
    add_compile_options(-Wno-misleading-indentation)
endif()

add_subdirectory(nrt)
add_subdirectory(nitf)
add_subdirectory(cgm)

if (ENABLE_J2K)
    add_subdirectory(j2k)
endif()

add_subdirectory(jpeg)

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
    #add_compile_options(-Wall -pedantic -Wextra)
    add_compile_options(-Wall -Wextra)
endif()

add_subdirectory(nitf)

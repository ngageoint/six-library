# turn on maximum warnings
if (MSVC)
    # By default, there is a /W3 on the command-line from somewhere (?); adding
    # /Wn results in a compiler warning.
    #
    # https://github.com/microsoft/STL/wiki/Changelog#vs-2022-179-preview-1
    # > *Note*: `/Wall` is not intended for regular production use, as it contains a large number of
    # > extremely noisy and low-value warnings. In general, the STL does not attempt to be `/Wall` clean.
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}") # add_compile_options(/W4)

    add_compile_options(/wd4996) # '...': This function or variable may be unsafe. Consider using sprintf_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
    add_compile_options(/wd4127) # conditional expression is constant
    add_compile_options(/wd4244) # '...': conversion from '...' to '...', possible loss of data
    add_compile_options(/wd4267) # '...': conversion from '...' to '...', possible loss of data
    
elseif (UNIX)
    #add_compile_options(-Wall -pedantic -Wextra)
    add_compile_options(-Wall -Wextra)

    add_compile_options(-Wno-deprecated-copy)
    add_compile_options(-Wno-class-memaccess)
    add_compile_options(-Wno-sizeof-pointer-div)
    add_compile_options(-Wno-unused-parameter)
    add_compile_options(-Wno-ignored-qualifiers)
endif()

add_subdirectory(scene)
add_subdirectory(six)
add_subdirectory(six.sicd)
add_subdirectory(six.sidd)
add_subdirectory(cphd)
add_subdirectory(cphd03)
add_subdirectory(six.convert)
add_subdirectory(samples)

# turn on maximum warnings
if (MSVC)
    add_compile_options(/std:c++14)

    # By default, there is a /W3 on the command-line from somewhere (?); adding
    # /Wn results in a compiler warning.
    #add_compile_options(/W4) # /Wall
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}") # /Wall

elseif (UNIX)
    add_compile_options(-std=c++14)

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

set(TARGET_LANGUAGE c++)

if (MSVC)
    # By default, there is a /W3 on the command-line from somewhere (?); adding
    # /W4 results in a compiler warning.
    #add_compile_options(/W4) # /Wall
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}") # /Wall

elseif (UNIX)
    #add_compile_options(-Wall -pedantic -Wextra)
    add_compile_options(-Wall -Wextra)
endif()

add_subdirectory(nitf)

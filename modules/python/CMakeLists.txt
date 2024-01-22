set(TARGET_LANGUAGE c++)

if (MSVC)
    # By default, there is a /W3 on the command-line from somewhere (?); adding
    # /W4 results in a compiler warning.
    #add_compile_options(/W0)
    string(REGEX REPLACE "/W[0-4]" "/W0" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
elseif (UNIX)
    add_compile_options(-w) # "Inhibit all warning messages"
endif()

add_subdirectory("nitf")

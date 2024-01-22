set(TARGET_LANGUAGE c++)

# Turn off all warnings; this is code we don't control.
if (MSVC)
    # By default, there is a /W3 on the command-line from somewhere (?); adding
    # /Wn results in a compiler warning.
    #add_compile_options(/W0)
    string(REGEX REPLACE "/W[0-4]" "/W0" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
elseif (UNIX)
    add_compile_options(-w) # "Inhibit all warning messages"
endif()

add_subdirectory("config")
add_subdirectory("except")
add_subdirectory("sys")
add_subdirectory("types")
add_subdirectory("mem")
add_subdirectory("mt")
add_subdirectory("io")
add_subdirectory("logging")
add_subdirectory("sio.lite")
add_subdirectory("math.linear")
add_subdirectory("math.poly")
add_subdirectory("xml.lite")

add_subdirectory("test_utils")

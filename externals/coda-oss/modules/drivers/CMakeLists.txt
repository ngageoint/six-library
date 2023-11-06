# Turn off all warnings; this is code we don't control.
if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)

    # By default, there is a /W3 on the command-line from somewhere (?); adding
    # /Wn results in a compiler warning.
    #add_compile_options(/W0)
    string(REGEX REPLACE "/W[0-4]" "/W0" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REGEX REPLACE "/W[0-4]" "/W0" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
elseif (UNIX)
    add_compile_options(-w) # "Inhibit all warning messages"
endif()

#add_subdirectory("curl") # this is handled in coda_find_system_dependencies

add_subdirectory("xml")

#add_subdirectory("gsl") # nothing to do

add_subdirectory("j2k")
add_subdirectory("jpeg")

add_subdirectory("uuid")

add_subdirectory("zlib")

add_subdirectory("hdf5")
add_subdirectory("highfive")

add_subdirectory("libtiff")

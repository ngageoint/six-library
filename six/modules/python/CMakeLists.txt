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

add_subdirectory(scene)
add_subdirectory(six)
add_subdirectory(six.sicd)
add_subdirectory(cphd03)
add_subdirectory(cphd)

# generate and install an empty __init__.py for the pysix package
file(GENERATE OUTPUT "__init__.py" CONTENT "")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/__init__.py"
        DESTINATION "${CODA_PYTHON_SITE_PACKAGES}/pysix"
        ${CODA_INSTALL_OPTION})

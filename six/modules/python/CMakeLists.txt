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

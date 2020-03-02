set(TARGET_LANG "python")

find_package(SWIG)

if(SWIG_FOUND AND Python_FOUND AND EXISTS(${CODA_INSTALL_DIR}/include/numpyutils/numpyutils.h))
    add_subdirectory("nitf")
else()
    message(WARNING "Unable to build python layer")
endif()

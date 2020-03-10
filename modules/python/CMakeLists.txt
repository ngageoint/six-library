set(TARGET_LANG "python")

find_package(SWIG)

if(SWIG_FOUND AND Python_FOUND)
    add_subdirectory("nitf")
else()
    message(WARNING "Unable to build python layer")
endif()

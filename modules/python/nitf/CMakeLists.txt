if (TARGET numpyutils-c++)
    coda_add_swig_python_module(
        TARGET nitf-python
        PACKAGE nitf
        MODULE_NAME nitropy
        MODULE_DEPS nitf-c numpyutils-c++ sys-c++ except-c++ io-c++ str-c++
        INPUT "source/nitro.i")

    if (CODA_BUILD_TESTS)
        # Experimental
        # Code below needs to be generalized and ported into coda_add_swig_python_module
        # and coda_add_tests

        # Copy over the python wrapper files into the same directory
        # as the swig module above
        # This ensures that the BINARY_DIR matches the contents
        #    of what will be installed
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/source/__init__.py"
           ${CMAKE_CURRENT_BINARY_DIR}
           COPYONLY
        )
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/source/generated/nitropy.py"
            ${CMAKE_CURRENT_BINARY_DIR}
            COPYONLY
        )

        # Get the parent of the bin dir for use as the PYTHONPATH
        get_filename_component(PYTHONPATH ${CMAKE_CURRENT_BINARY_DIR} DIRECTORY)

        include(FindPython3)
        file(GLOB local_tests ${CMAKE_CURRENT_LIST_DIR}/unittests/*.py)
        foreach(test_src ${local_tests})
            get_filename_component(test_file "${test_src}" NAME)
            get_filename_component(test_name "${test_src}" NAME_WE)
            set(test_target "nitropy_${test_name}")

            add_test(NAME ${test_target}
                COMMAND ${Python3_EXECUTABLE} ${test_file}
                WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/unittests
            )
            # Set the PYTHONPATH on the test so that it can see the module
            set_tests_properties(${test_target}
                PROPERTIES ENVIRONMENT "PYTHONPATH=${PYTHONPATH}")
        endforeach()
    endif()

endif()

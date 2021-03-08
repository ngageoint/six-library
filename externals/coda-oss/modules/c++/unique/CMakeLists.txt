if (UUID_LIB)
    set(MODULE_NAME unique)

    coda_add_module(
        ${MODULE_NAME}
        VERSION 1.0
        DEPS except-c++ ${UUID_LIB})

    coda_add_tests(
        MODULE_NAME ${MODULE_NAME}
        DIRECTORY "tests")
endif()

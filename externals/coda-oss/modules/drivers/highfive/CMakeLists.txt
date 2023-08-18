set(MODULE_NAME highfive)
set(TARGET_LANGUAGE c++)

if(CODA_ENABLE_HDF5)
    set(MODULE_DEPS config-c++ sys-c++)
    list(APPEND MODULE_DEPS hdf5-c++)

    coda_add_module(
        ${MODULE_NAME}
        VERSION 2.7.1
        DEPS ${MODULE_DEPS})

    coda_add_tests(
        MODULE_NAME ${MODULE_NAME}
        DIRECTORY "unittests"
        UNITTEST)
else()
    message("${MODULE_NAME} will not be built since HDF5 is not enabled")
endif()

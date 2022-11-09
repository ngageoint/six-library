set(MODULE_NAME hdf5.lite)

if(CODA_ENABLE_HDF5)
    set(MODULE_DEPS str-c++ except-c++ types-c++ io-c++ logging-c++ str-c++ coda_oss-c++ config-c++ gsl-c++)
    list(APPEND MODULE_DEPS hdf5-c++)

    coda_add_module(
        ${MODULE_NAME}
        VERSION 1.0
        DEPS ${MODULE_DEPS})

    coda_add_tests(
        MODULE_NAME ${MODULE_NAME}
        DIRECTORY "unittests"
        UNITTEST)
else()
    message("${MODULE_NAME} will not be built since HDF5 is not enabled")
endif()

set(MODULE_NAME hdf5.lite)

set(MODULE_DEPS highfive-c++ hdf5-c++)
list(APPEND MODULE_DEPS except-c++ types-c++ io-c++ coda_oss-c++)

coda_add_module(
    ${MODULE_NAME}
    VERSION 1.0
    DEPS ${MODULE_DEPS})

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "unittests"
    UNITTEST)


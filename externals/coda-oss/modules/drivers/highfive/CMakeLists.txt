set(MODULE_NAME highfive)
set(TARGET_LANGUAGE c++)

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


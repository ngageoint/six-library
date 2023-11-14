set(MODULE_NAME re)
set(MODULE_DEPS sys-c++)

coda_add_module(
    ${MODULE_NAME}
    VERSION 1.0
    DEPS ${MODULE_DEPS})

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "unittests"
    UNITTEST)


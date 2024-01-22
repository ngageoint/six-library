set(MODULE_NAME units)

coda_add_module(${MODULE_NAME}
    VERSION 1.0
    DEPS math-c++)

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "unittests"
    DEPS sys-c++
    UNITTEST)

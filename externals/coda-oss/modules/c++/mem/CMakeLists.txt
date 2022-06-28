set(MODULE_NAME mem)

coda_add_module(
    ${MODULE_NAME}
    VERSION 1.0
    DEPS sys-c++ gsl-c++)

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "tests"
    DEPS cli-c++)
coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "unittests"
    DEPS std-c++
    UNITTEST)

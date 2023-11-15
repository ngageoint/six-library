set(MODULE_NAME io)

coda_add_module(
    ${MODULE_NAME}
    VERSION 1.0
    DEPS sys-c++ mem-c++ std-c++ gsl-c++)

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "unittests"
    UNITTEST)

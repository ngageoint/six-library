set(MODULE_NAME math.linear)

coda_add_module(
    ${MODULE_NAME}
    VERSION 0.2
    DEPS sys-c++ mem-c++ types-c++ gsl-c++)

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "unittests"
    DEPS "${UNITTEST_DEPS}"
    UNITTEST)

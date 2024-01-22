set(MODULE_NAME math.poly)

coda_add_module(
    ${MODULE_NAME}
    VERSION 0.2
    DEPS sys-c++ math.linear-c++)

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "unittests"
    DEPS "${UNITTEST_DEPS}"
    UNITTEST)

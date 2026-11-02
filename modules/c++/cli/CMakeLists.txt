set(MODULE_NAME "cli")

coda_add_module(
    ${MODULE_NAME}
    VERSION 1.2
    DEPS sys-c++ mem-c++)

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "unittests"
    UNITTEST)

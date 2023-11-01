set(MODULE_NAME polygon)

coda_add_module(
    ${MODULE_NAME}
    VERSION 1.0
    DEPS sys-c++ mem-c++ types-c++ math-c++ except-c++)

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "unittests"
    UNITTEST)

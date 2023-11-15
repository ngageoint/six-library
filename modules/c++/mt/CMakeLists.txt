set(MODULE_NAME mt)

coda_add_module(
    ${MODULE_NAME}
    VERSION 1.1
    DEPS sys-c++ except-c++ math-c++ mem-c++ types-c++ std-c++)

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "unittests"
    UNITTEST)

set(MODULE_NAME math)

coda_add_module(
    ${MODULE_NAME}
    VERSION 0.1
    DEPS except-c++ str-c++ sys-c++ types-c++ coda_oss-c++)

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "unittests"
    DEPS std-c++
    UNITTEST)

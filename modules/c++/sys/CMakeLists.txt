set(MODULE_NAME sys)

coda_add_module(
    ${MODULE_NAME}
    VERSION 1.2
    DEPS config-c++ except-c++ str-c++ gsl-c++ coda_oss-c++)

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "unittests"
    DEPS mt-c++ std-c++
    UNITTEST)

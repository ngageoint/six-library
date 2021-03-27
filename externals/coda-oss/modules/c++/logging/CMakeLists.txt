set(MODULE_NAME logging)

coda_add_module(
    ${MODULE_NAME}
    VERSION 1.1
    DEPS io-c++ mt-c++ std-c++ sys-c++ coda_oss-c++)

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "tests")
coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "unittests"
    UNITTEST)

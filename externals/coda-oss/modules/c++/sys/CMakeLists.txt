set(MODULE_NAME sys)

coda_generate_module_config_header(${MODULE_NAME})

coda_add_module(
    ${MODULE_NAME}
    VERSION 1.2
    DEPS config-c++ except-c++ str-c++)

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "tests"
    FILTER_LIST "MMapReadOnlyTest.cpp" "ProcessTest1.cpp")
coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "unittests"
    DEPS mt-c++ std-c++
    UNITTEST)

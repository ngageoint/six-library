set(MODULE_NAME avx)

coda_generate_module_config_header(${MODULE_NAME})

coda_add_module(
    ${MODULE_NAME}
    VERSION 1.2
    DEPS config-c++)

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "unittests"
    DEPS std-c++ sys-c++
    UNITTEST)

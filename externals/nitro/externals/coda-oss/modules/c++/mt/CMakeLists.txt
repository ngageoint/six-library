set(MODULE_NAME mt)

set(MT_DEFAULT_PINNING OFF CACHE BOOL
    "Use affinity-based CPU pinning by default in MT")
coda_generate_module_config_header(${MODULE_NAME})

coda_add_module(
    ${MODULE_NAME}
    VERSION 1.1
    DEPS sys-c++ except-c++ math-c++ mem-c++ types-c++)

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "tests"
    DEPS cli-c++)
coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "unittests"
    UNITTEST)

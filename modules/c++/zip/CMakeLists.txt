set(MODULE_NAME zip)

coda_add_module(
    ${MODULE_NAME}
    VERSION 1.0
    DEPS io-c++ coda_oss-c++ z minizip)

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "unittests"
    UNITTEST)

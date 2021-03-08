if (TARGET z AND TARGET minizip)
    set(MODULE_NAME zip)

    coda_add_module(
        ${MODULE_NAME}
        VERSION 1.0
        DEPS io-c++ z minizip)

    coda_add_tests(
        MODULE_NAME ${MODULE_NAME}
        DIRECTORY "tests")
endif()

if (ENABLE_ZIP AND CONAN_PACKAGE_NAME)
    # import targets from zlib conan package
    find_package(coda-oss_zlib REQUIRED)
endif()

if (TARGET z AND TARGET minizip)
    set(MODULE_NAME zip)

    coda_add_module(
        ${MODULE_NAME}
        VERSION 1.0
        DEPS io-c++ coda_oss-c++ z minizip)

    coda_add_tests(
        MODULE_NAME ${MODULE_NAME}
        DIRECTORY "tests")

    coda_add_tests(
        MODULE_NAME ${MODULE_NAME}
        DIRECTORY "unittests"
        UNITTEST)
else()
    message("zip will not be build since zlib + minizip were not enabled")
endif()

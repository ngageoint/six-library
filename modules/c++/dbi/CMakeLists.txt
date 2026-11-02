set(MODULE_NAME dbi)

if (MSVC AND (MSVC_VERSION LESS 1920))
    add_compile_options(/wd4702) # disable warning 'unreachable code'
endif()

coda_add_module(
    ${MODULE_NAME}
    VERSION 1.0
    DEPS io-c++ ${SQL_LIB}) # see modules/drivers/sql

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "tests")

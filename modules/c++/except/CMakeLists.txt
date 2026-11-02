set(MODULE_NAME except)

if (MSVC AND (MSVC_VERSION LESS_EQUAL 1900))
    add_compile_options(/wd4091) # 'typedef ' ignored on left of '' when no variable is declared
endif()

coda_add_module(
    ${MODULE_NAME}
    VERSION 1.0
    DEPS config-c++)

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "tests")

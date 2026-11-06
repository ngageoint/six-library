set(MODULE_NAME str)

coda_add_module(
    ${MODULE_NAME}
    VERSION 1.0
    DEPS types-c++ except-c++ coda_oss-c++)

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "tests")
coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "unittests"
    DEPS sys-c++ std-c++
    UNITTEST)

target_compile_options(${MODULE_NAME}_test_base_convert PRIVATE
    "$<$<CXX_COMPILER_ID:Clang>:-Wno-unused-function>")

set(MODULE_NAME sys)

include(CheckSourceCompiles)
check_source_compiles(CXX [[ int main() { __builtin_cpu_init(); return 0; } ]] HAVE_BUILTIN_CPU_INIT)
coda_generate_module_config_header(${MODULE_NAME})

coda_add_module(
    ${MODULE_NAME}
    VERSION 1.2
    DEPS config-c++ except-c++ str-c++ gsl-c++ coda_oss-c++)

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "tests"
    FILTER_LIST "MMapReadOnlyTest.cpp" "ProcessTest1.cpp")
coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "unittests"
    DEPS mt-c++ std-c++
    UNITTEST)

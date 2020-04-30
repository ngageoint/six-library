set(MODULE_NAME math)

check_symbol_exists("isnan" "math.h" HAVE_ISNAN)
# The auto-generated test code doesn't work for overloaded functions
check_cxx_source_compiles("
    #include <cmath>
    int main() { return std::isnan(0.0); }"
    HAVE_STD_ISNAN)
coda_generate_module_config_header(${MODULE_NAME})

coda_add_module(
    ${MODULE_NAME}
    VERSION 0.1
    DEPS except-c++ str-c++ sys-c++ types-c++)

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "tests")
coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "unittests"
    UNITTEST)

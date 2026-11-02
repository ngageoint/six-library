set(MODULE_NAME coda_oss.json)

coda_add_module(${MODULE_NAME}
    VERSION 1.0
    DEPS math.linear-c++ math.poly-c++ types-c++ mem-c++ nlohmann-c++)

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "unittests"
    UNITTEST
    SOURCES 
        test_json_math.cpp  
        test_json_mem.cpp  
        test_json_std.cpp  
        test_json_types.cpp)

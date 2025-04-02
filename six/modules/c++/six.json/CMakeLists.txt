set(MODULE_NAME six.json)

coda_add_module(${MODULE_NAME} VERSION 1.0
    DEPS scene-c++ six-c++ six.sicd-c++ cphd03-c++ cphd-c++ coda_oss.json-c++ nlohmann-c++)

coda_add_tests(MODULE_NAME ${MODULE_NAME} 
    DIRECTORY "unittests" 
    UNITTEST 
    SOURCES test_json_six.cpp)
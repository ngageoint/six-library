set(MODULE_NAME xml.lite)

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-zero-as-null-pointer-constant")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-suggest-override")
endif()

set(MODULE_DEPS io-c++ mem-c++ mt-c++ logging-c++ str-c++ coda_oss-c++)
list(APPEND MODULE_DEPS xerces-c)

coda_add_module(
    ${MODULE_NAME}
    VERSION 1.2
    DEPS ${MODULE_DEPS})

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "unittests"
    UNITTEST)


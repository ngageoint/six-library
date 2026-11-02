set(MODULE_NAME xml.lite)

if(ENABLE_XML)
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
       add_compile_options(-Wno-zero-as-null-pointer-constant -Wno-suggest-override)
    endif()

    if(CONAN_PACKAGE_NAME)
        # import targets from xerces-c conan package
        find_package(coda-oss_xerces-c REQUIRED)
        set(USE_XercesC 1)
    endif()

    set(MODULE_DEPS io-c++ mem-c++ mt-c++ logging-c++ str-c++ coda_oss-c++)
    if (USE_XercesC)
        list(APPEND MODULE_DEPS xerces-c)
    endif()

    coda_generate_module_config_header(${MODULE_NAME})

    coda_add_module(
        ${MODULE_NAME}
        VERSION 1.2
        DEPS ${MODULE_DEPS})

    coda_add_tests(
        MODULE_NAME ${MODULE_NAME}
        DIRECTORY "tests"
        DEPS cli-c++
        FILTER_LIST "MMParserTest1.cpp" "MinidomParserTest2.cpp")

    coda_add_tests(
        MODULE_NAME ${MODULE_NAME}
        DIRECTORY "unittests"
        UNITTEST)

    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/unittests/"
        DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/unittests"
        FILES_MATCHING PATTERN "*.xml")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/unittests/embedded_xml.bin"
        DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/unittests")
else()
    message("${MODULE_NAME} will not be built since XML is not enabled")
endif()

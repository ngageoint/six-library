if (TARGET re-c++)
    set(MODULE_NAME net)
    set(MODULE_DEPS logging-c++ re-c++ io-c++ mem-c++)
    if (CURL_FOUND)
        list(APPEND MODULE_DEPS CURL::libcurl)  # From FindCURL
        set(NET_CURL_SUPPORT "1")
    endif()
    coda_generate_module_config_header(${MODULE_NAME})

    coda_add_module(
        ${MODULE_NAME}
        VERSION 1.0
        DEPS ${MODULE_DEPS})

    coda_add_tests(
        MODULE_NAME ${MODULE_NAME}
        DIRECTORY "tests"
        FILTER_LIST "AckMulticastSender.cpp" "AckMulticastSubscriber.cpp"
                    "MulticastSender.cpp" "MulticastSubscriber.cpp"
                    "SerializableTestClient.cpp")
endif()

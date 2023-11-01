set(MODULE_NAME net.ssl)
set(MODULE_DEPS net-c++)

if (OPENSSL_FOUND)
    set(USE_OPENSSL 1)
    list(APPEND MODULE_DEPS OpenSSL::SSL OpenSSL::Crypto)
endif()
coda_generate_module_config_header(${MODULE_NAME})

coda_add_module(
    ${MODULE_NAME}
    VERSION 1.0
    DEPS ${MODULE_DEPS})

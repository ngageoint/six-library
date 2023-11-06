set(MODULE_NAME tiff)

coda_add_module(
    ${MODULE_NAME}
    VERSION 1.0
    DEPS libtiff-c mt-c++ io-c++)


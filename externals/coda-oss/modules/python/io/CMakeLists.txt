coda_add_swig_python_module(
    TARGET io-python
    PACKAGE coda
    MODULE_NAME coda_io
    MODULE_DEPS io-c++
    INPUT "source/io.i")

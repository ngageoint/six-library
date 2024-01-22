coda_add_swig_python_module(
    TARGET io-python
    PACKAGE coda
    MODULE_NAME coda_io
    MODULE_DEPS io-c++
    PYTHON_DEPS config-python
    INPUT "source/io.i")

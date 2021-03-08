coda_add_swig_python_module(
    TARGET except-python
    PACKAGE coda
    MODULE_NAME coda_except
    MODULE_DEPS except-c++
    INPUT "source/except.i")

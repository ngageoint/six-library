coda_add_swig_python_module(
    TARGET sys-python
    PACKAGE coda
    MODULE_NAME coda_sys
    MODULE_DEPS sys-c++
    INPUT "source/sys.i")

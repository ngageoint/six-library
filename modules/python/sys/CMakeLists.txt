coda_add_swig_python_module(
    TARGET sys-python
    PACKAGE coda
    MODULE_NAME coda_sys
    MODULE_DEPS sys-c++
    PYTHON_DEPS config-python
    INPUT "source/sys.i")

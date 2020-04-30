coda_add_swig_python_module(
    TARGET types-python
    PACKAGE coda
    MODULE_NAME coda_types
    MODULE_DEPS types-c++ sys-c++
    PYTHON_DEPS sys-python
    INPUT "source/types.i")

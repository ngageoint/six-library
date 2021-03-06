coda_add_swig_python_module(
    TARGET mt-python
    PACKAGE coda
    MODULE_NAME mt
    MODULE_DEPS mt-c++ sys-c++
    PYTHON_DEPS except-python mem-python types-python
    INPUT "source/mt.i")

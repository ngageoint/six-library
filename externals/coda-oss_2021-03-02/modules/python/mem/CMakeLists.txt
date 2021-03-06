coda_add_swig_python_module(
    TARGET mem-python
    PACKAGE coda
    MODULE_NAME mem
    MODULE_DEPS mem-c++
    INPUT "source/mem.i")

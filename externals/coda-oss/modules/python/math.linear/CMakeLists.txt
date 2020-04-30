coda_add_swig_python_module(
    TARGET math.linear-python
    PACKAGE coda
    MODULE_NAME math_linear
    MODULE_DEPS math.linear-c++
    PYTHON_DEPS except-python types-python
    INPUT "source/math_linear.i")

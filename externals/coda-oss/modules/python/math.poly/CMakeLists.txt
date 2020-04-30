if (TARGET numpyutils-c++)
    coda_add_swig_python_module(
        TARGET math.poly-python
        PACKAGE coda
        MODULE_NAME math_poly
        MODULE_DEPS math.poly-c++ numpyutils-c++
        PYTHON_DEPS except-python math.linear-python types-python
        INPUT "source/math_poly.i")
endif()

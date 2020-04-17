coda_add_swig_python_module(
    TARGET six.sicd-python
    PACKAGE pysix
    MODULE_NAME six_sicd
    MODULE_DEPS six.sicd-c++ six-c++ numpyutils-c++ nitf-c nrt-c
    PYTHON_DEPS six-python
    INPUT "source/six_sicd.i")

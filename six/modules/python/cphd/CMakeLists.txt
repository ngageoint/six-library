if (TARGET six.sicd-python)
    coda_add_swig_python_module(
        TARGET cphd-python
        PACKAGE pysix
        MODULE_NAME cphd
        MODULE_DEPS cphd-c++ six.sicd-c++ six-c++ sys-c++ types-c++ numpyutils-c++
        PYTHON_DEPS types-python mem-python six.sicd-python six-python sys-python
        INPUT "source/cphd.i")
endif()

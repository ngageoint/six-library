if (TARGET six.sicd-python)
    coda_add_swig_python_module(
        TARGET cphd03-python
        PACKAGE pysix
        MODULE_NAME cphd03
        MODULE_DEPS cphd03-c++ cphd-c++ six.sicd-c++ six-c++ sys-c++ types-c++
        PYTHON_DEPS types-python mem-python six.sicd-python six-python sys-python
        INPUT "source/cphd03.i")
endif()

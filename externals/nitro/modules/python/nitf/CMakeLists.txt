if (TARGET numpyutils-c++)
    coda_add_swig_python_module(
        TARGET nitf-python
        PACKAGE nitf
        MODULE_NAME nitropy
        MODULE_DEPS nitf-c numpyutils-c++ sys-c++ except-c++ io-c++ str-c++
        INPUT "source/nitro.i")
endif()

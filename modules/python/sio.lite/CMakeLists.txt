coda_add_swig_python_module(
    TARGET sio.lite-python
    PACKAGE coda
    MODULE_NAME sio_lite
    MODULE_DEPS sio.lite-c++
    PYTHON_DEPS config-python io-python sys-python except-python
    INPUT "source/sio_lite.i")

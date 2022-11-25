coda_add_swig_python_module(
    TARGET logging-python
    PACKAGE coda
    MODULE_NAME coda_logging
    MODULE_DEPS logging-c++
    PYTHON_DEPS config-python
    INPUT "source/logging.i")

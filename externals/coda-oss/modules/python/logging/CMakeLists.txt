coda_add_swig_python_module(
    TARGET logging-python
    PACKAGE coda
    MODULE_NAME coda_logging
    MODULE_DEPS logging-c++
    INPUT "source/logging.i")

coda_add_swig_python_module(
    TARGET xml.lite-python
    PACKAGE coda
    MODULE_NAME xml_lite
    MODULE_DEPS xml.lite-c++
    INPUT "source/xml_lite.i")

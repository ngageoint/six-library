coda_add_swig_python_module(
    TARGET six-python
    PACKAGE pysix
    MODULE_NAME six_base
    MODULE_DEPS six-c++ nitf-c++ numpyutils-c++
    PYTHON_DEPS scene-python math.poly-python except-python xml.lite-python
                logging-python io-python mem-python types-python sio.lite-python
    INPUT "source/six.i")

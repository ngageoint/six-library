if (TARGET math.poly-python)
    coda_add_swig_python_module(
        TARGET scene-python
        PACKAGE pysix
        MODULE_NAME scene
        MODULE_DEPS scene-c++
        PYTHON_DEPS math.linear-python math.poly-python types-python except-python
        INPUT "source/scene.i")
endif()

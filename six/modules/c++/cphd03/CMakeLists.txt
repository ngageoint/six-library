set(MODULE_NAME cphd03)

coda_add_module(
    ${MODULE_NAME}
    DEPS cphd-c++
    SOURCES
        source/Antenna.cpp
        source/CPHDReader.cpp
        source/CPHDWriter.cpp
        source/CPHDXMLControl.cpp
        source/Channel.cpp
        source/Data.cpp
        source/FileHeader.cpp
        source/Global.cpp
        source/Metadata.cpp
        source/SRP.cpp
        source/Utilities.cpp
        source/VBM.cpp
        source/VectorParameters.cpp)

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "unittests"
    UNITTEST
    SOURCES
        test_cphd_read_unscaled_int.cpp
        test_cphd_write.cpp
        test_vbm.cpp)

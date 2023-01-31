coda_add_module(
    six.sidd30
    DEPS tiff-c++ six-c++ six.sidd-c++
    SOURCES
        source/CompressedSIDDByteProvider.cpp
        source/DerivedClassification.cpp
        source/DerivedDataBuilder.cpp
        source/DerivedXMLControl.cpp
        source/DerivedXMLParser300.cpp
        source/GeoTIFFReadControl.cpp
        source/SIDDByteProvider.cpp
        source/SIDDVersionUpdater.cpp
        source/Utilities.cpp)

coda_add_tests(
    MODULE_NAME six.sidd30
    DIRECTORY "tests"
    DEPS cli-c++
    SOURCES
        test_byte_swap.cpp
        test_check_blocking.cpp
        test_geotiff.cpp
        test_read_and_write_lut.cpp
        test_sidd_blocking.cpp
        test_sidd_byte_provider.cpp)

coda_add_tests(
    MODULE_NAME six.sidd30
    DIRECTORY "unittests"
    UNITTEST
    SOURCES
        test_read_sidd_legend.cpp
        test_valid_sixsidd.cpp
        unittest_sidd_byte_provider.cpp)

# Install the schemas
install(DIRECTORY "conf/schema/"
        DESTINATION "conf/schema/six/")

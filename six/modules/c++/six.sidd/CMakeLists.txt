coda_add_module(
    six.sidd
    DEPS tiff-c++ six-c++
    SOURCES
        source/CompressedSIDDByteProvider.cpp
        source/Compression.cpp
        source/CropUtils.cpp
        source/DerivedClassification.cpp
        source/DerivedData.cpp
        source/DerivedDataBuilder.cpp
        source/DerivedXMLControl.cpp
        source/DerivedXMLParser.cpp
        source/DerivedXMLParser100.cpp
        source/DerivedXMLParser200.cpp
        source/DerivedXMLParser300.cpp
        source/DigitalElevationData.cpp
        source/Display.cpp
        source/DownstreamReprocessing.cpp
        source/ExploitationFeatures.cpp
        source/Filter.cpp
        source/GeoTIFFReadControl.cpp
        source/GeoTIFFWriteControl.cpp
        source/GeographicAndTarget.cpp
        source/LookupTable.cpp
        source/Measurement.cpp
        source/ProductCreation.cpp
        source/SFA.cpp
        source/SIDDByteProvider.cpp
        source/SIDDVersionUpdater.cpp
        source/Utilities.cpp)

coda_add_tests(
    MODULE_NAME six.sidd
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
    MODULE_NAME six.sidd
    DIRECTORY "unittests"
    UNITTEST
    SOURCES
        test_annotations_equality.cpp
        test_geometric_chip.cpp
        test_read_sidd_legend.cpp
        test_valid_sixsidd.cpp
        unittest_sidd_byte_provider.cpp)

# Install the schemas
install(DIRECTORY "conf/schema/"
        DESTINATION "conf/schema/six/")

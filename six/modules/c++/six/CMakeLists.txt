coda_add_module(
    six
    DEPS XML_DATA_CONTENT-static-c nitf-c++
         scene-c++ logging-c++ xml.lite-c++ sys-c++ str-c++
         except-c++ types-c++ config-c++ gsl-c++ std-c++
         ${CMAKE_DL_LIBS}
    SOURCES
        source/Adapters.cpp
        source/ByteProvider.cpp
        source/Classification.cpp
        source/CollectionInformation.cpp
        source/CompressedByteProvider.cpp
        source/Container.cpp
        source/Data.cpp
        source/ErrorStatistics.cpp
        source/GeoDataBase.cpp
        source/GeoInfo.cpp
        source/Init.cpp
        source/Logger.cpp
        source/MatchInformation.cpp
        source/Mesh.cpp
        source/NITFHeaderCreator.cpp
        source/NITFImageInfo.cpp
        source/NITFImageInputStream.cpp
        source/NITFReadControl.cpp
        source/NITFSegmentInfo.cpp
        source/NITFWriteControl.cpp
        source/Options.cpp
        source/ParameterCollection.cpp
        source/Radiometric.cpp
        source/ReadControlFactory.cpp
        source/SICommonXMLParser.cpp
        source/SICommonXMLParser01x.cpp
        source/SICommonXMLParser10x.cpp
        source/Types.cpp
        source/Utilities.cpp
        source/VersionUpdater.cpp
        source/WriteControl.cpp
        source/XmlLite.cpp
        source/XMLControl.cpp
        source/XMLControlFactory.cpp
        source/XMLParser.cpp)


set(DEFAULT_SCHEMA_PATH "${CMAKE_INSTALL_PREFIX}/conf/schema/six")
target_compile_definitions(six-c++ PRIVATE
                           DEFAULT_SCHEMA_PATH="${DEFAULT_SCHEMA_PATH}")

coda_add_tests(
    MODULE_NAME six
    DIRECTORY "tests"
    SOURCES
        test_determine_data_type.cpp
        test_parameter_collection.cpp)

coda_add_tests(
    MODULE_NAME six
    DIRECTORY "unittests"
    UNITTEST
    SOURCES
        test_fft_sign_conversions.cpp
        test_polarization_type_conversions.cpp
        test_serialize.cpp
        test_xml_control.cpp)

if(CODA_BUILD_TESTS)
    target_compile_definitions(six_test_xml_control PRIVATE
                               DEFAULT_SCHEMA_PATH="${DEFAULT_SCHEMA_PATH}")
endif()

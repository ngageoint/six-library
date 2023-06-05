set(MODULE_NAME cphd)

coda_add_module(
    cphd
    DEPS mt-c++ six.sicd-c++
    SOURCES
        source/Antenna.cpp
        source/BaseFileHeader.cpp
        source/ByteSwap.cpp
        source/CPHDReader.cpp
        source/CPHDWriter.cpp
        source/CPHDXMLControl.cpp
        source/CPHDXMLParser.cpp
        source/Channel.cpp
        source/Data.cpp
        source/DataWriter.cpp
        source/Dwell.cpp
        source/ErrorParameters.cpp
        source/FileHeader.cpp
        source/Global.cpp
        source/Metadata.cpp
        source/PVP.cpp
        source/PVPBlock.cpp
        source/ProductInfo.cpp
        source/ReferenceGeometry.cpp
        source/SceneCoordinates.cpp
        source/SupportArray.cpp
        source/SupportBlock.cpp
        source/TestDataGenerator.cpp
        source/TxRcv.cpp
        source/Utilities.cpp
        source/Wideband.cpp)

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "tests"
    DEPS cli-c++
    SOURCES
        test_compare_cphd.cpp
        test_metadata_round.cpp
        test_round_trip.cpp)

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "unittests"
    UNITTEST
    SOURCES
        test_channel.cpp
        test_compressed_signal_block_round.cpp
        test_cphd_xml_control.cpp
        test_cphd_xml_optional.cpp
        test_dwell.cpp
        test_file_header.cpp
        test_pvp.cpp
        test_pvp_block.cpp
        test_pvp_block_round.cpp
        test_read_wideband.cpp
        test_reference_geometry.cpp
        test_signal_block_round.cpp
        test_support_block_round.cpp)

# Install the schemas
file(GLOB cphd_schemas "${CMAKE_CURRENT_SOURCE_DIR}/conf/schema/*")
install(FILES ${cphd_schemas} DESTINATION "conf/schema/six")

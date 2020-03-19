add_library(cphd-c++ source/Antenna.cpp
                     source/BaseFileHeader.cpp
                     source/ByteSwap.cpp
                     source/CPHDReader.cpp
                     source/CPHDWriter.cpp
                     source/CPHDXMLControl.cpp
                     source/CPHDXMLParser.cpp
                     source/Channel.cpp
                     source/Data.cpp
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

common_module_config(cphd-c++)
target_link_libraries(cphd-c++ mt-c++ six.sicd-c++)

set(unittest_srcs test_channel
                  test_compressed_signal_block_round
                  test_cphd_xml_control
                  test_cphd_xml_optional
                  test_dwell
                  test_file_header
                  test_pvp
                  test_pvp_block
                  test_pvp_block_round
                  test_read_wideband
                  test_reference_geometry
                  test_signal_block_round
                  test_support_block_round)
add_unittests("${unittest_srcs}" cpp cphd-c++ cphd)

# Install the schemas
file(GLOB cphd_schemas ${CMAKE_CURRENT_SOURCE_DIR}/conf/schema/*)
install(FILES ${cphd_schemas} DESTINATION ${CMAKE_INSTALL_PREFIX}/conf/schema/six)

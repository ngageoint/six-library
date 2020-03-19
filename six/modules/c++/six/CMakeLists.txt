add_library(six-c++ source/Adapters.cpp
                    source/ByteProvider.cpp
                    source/Classification.cpp
                    source/CollectionInformation.cpp
                    source/CompressedByteProvider.cpp
                    source/Container.cpp
                    source/Data.cpp
                    source/ErrorStatistics.cpp
                    source/GeoInfo.cpp
                    source/Init.cpp
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
                    source/XMLControl.cpp
                    source/XMLControlFactory.cpp
                    source/XMLParser.cpp)

common_module_config(six-c++)
add_dependencies(six-c++ nitro)

target_link_libraries(six-c++ XML_DATA_CONTENT-static-c nitf-c++
scene-c++ logging-c++ xml.lite-c++ ${CMAKE_DL_LIBS})
target_compile_definitions(six-c++ PUBLIC
                           DEFAULT_SCHEMA_PATH="${CMAKE_INSTALL_PREFIX}/conf/schema/six")

set(unittest_srcs test_fft_sign_conversions
                  test_polarization_type_conversions
                  test_serialize
                  test_xml_control)
add_unittests("${unittest_srcs}" cpp six-c++ six)

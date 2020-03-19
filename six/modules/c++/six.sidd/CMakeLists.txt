add_library(six.sidd-c++ source/CompressedSIDDByteProvider.cpp
                         source/Compression.cpp
                         source/CropUtils.cpp
                         source/DerivedClassification.cpp
                         source/DerivedData.cpp
                         source/DerivedDataBuilder.cpp
                         source/DerivedXMLControl.cpp
                         source/DerivedXMLParser.cpp
                         source/DerivedXMLParser100.cpp
                         source/DerivedXMLParser200.cpp
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

common_module_config(six.sidd-c++)
target_link_libraries(six.sidd-c++ tiff-c++ six-c++)


set(unittest_srcs test_annotations_equality
                  test_geometric_chip
                  test_read_sidd_legend)
add_unittests("${unittest_srcs}" cpp six.sidd-c++ six.sidd)

# Install the schemas
file(GLOB sidd_schemas ${CMAKE_CURRENT_SOURCE_DIR}/conf/schema/*xsd)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/conf/schema/external DESTINATION ${CMAKE_INSTALL_PREFIX}/conf/schema/six)
install(FILES ${sidd_schemas} DESTINATION ${CMAKE_INSTALL_PREFIX}/conf/schema/six)


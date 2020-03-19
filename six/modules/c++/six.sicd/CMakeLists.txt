add_library(six.sicd-c++ source/Antenna.cpp
                         source/AreaPlaneUtility.cpp
                         source/ComplexData.cpp
                         source/ComplexDataBuilder.cpp
                         source/ComplexXMLControl.cpp
                         source/ComplexXMLParser.cpp
                         source/ComplexXMLParser040.cpp
                         source/ComplexXMLParser041.cpp
                         source/ComplexXMLParser04x.cpp
                         source/ComplexXMLParser050.cpp
                         source/ComplexXMLParser100.cpp
                         source/ComplexXMLParser101.cpp
                         source/ComplexXMLParser10x.cpp
                         source/CropUtils.cpp
                         source/Functor.cpp
                         source/GeoData.cpp
                         source/GeoLocator.cpp
                         source/Grid.cpp
                         source/ImageData.cpp
                         source/ImageFormation.cpp
                         source/PFA.cpp
                         source/Position.cpp
                         source/RMA.cpp
                         source/RadarCollection.cpp
                         source/RgAzComp.cpp
                         source/SCPCOA.cpp
                         source/SICDByteProvider.cpp
                         source/SICDMesh.cpp
                         source/SICDVersionUpdater.cpp
                         source/SICDWriteControl.cpp
                         source/SlantPlanePixelTransformer.cpp
                         source/Timeline.cpp
                         source/Utilities.cpp)
common_module_config(six.sicd-c++)
target_link_libraries(six.sicd-c++ six-c++)

set(unittest_srcs test_area_plane
                  test_filling_geo_data
                  test_filling_grid
                  test_filling_pfa
                  test_filling_position
                  test_filling_rgazcomp
                  test_filling_rma
                  test_filling_scpcoa
                  test_get_segment
                  test_projection_polynomial_fitter
                  test_radar_collection
                  test_update_sicd_version
                  test_utilities)
add_unittests("${unittest_srcs}" cpp six.sicd-c++ six.sicd)

# Install the schemas
file(GLOB sicd_schemas ${CMAKE_CURRENT_SOURCE_DIR}/conf/schema/*)
install(FILES ${sicd_schemas} DESTINATION ${CMAKE_INSTALL_PREFIX}/conf/schema/six)

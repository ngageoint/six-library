coda_add_module(
    six.sicd
    DEPS six-c++ units-c++
    SOURCES
        source/Antenna.cpp
        source/AreaPlaneUtility.cpp
        source/ComplexData.cpp
        source/ComplexDataBuilder.cpp
        source/ComplexToAMP8IPHS8I.cpp
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
        source/KDTree.cpp
        source/NITFReadComplexXMLControl.cpp
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

coda_add_tests(
    MODULE_NAME six.sicd
    DIRECTORY "tests"
    DEPS cli-c++
    SOURCES
        derive_output_plane.cpp
        test_add_additional_des.cpp
        test_clone_container.cpp
        test_compare_sicd_meshes.cpp
        test_get_complex_data.cpp
        test_load_from_input_stream.cpp
        test_mesh_polyfit.cpp
        test_mesh_roundtrip.cpp
        test_read_sicd_mesh.cpp
        test_read_sicd_with_extra_des.cpp
        test_read_sicd.cpp
        test_sicd_byte_provider.cpp
        test_sicd_schemata.cpp
        test_streaming_write.cpp
        test_vdp_polyfit.cpp)

coda_add_tests(
    MODULE_NAME six.sicd
    DIRECTORY "unittests"
    UNITTEST
    SOURCES
        test_area_plane.cpp
        test_filling_geo_data.cpp
        test_filling_grid.cpp
        test_filling_pfa.cpp
        test_filling_position.cpp
        test_filling_rgazcomp.cpp
        test_filling_rma.cpp
        test_filling_scpcoa.cpp
        test_get_segment.cpp
        test_projection_polynomial_fitter.cpp
        test_radar_collection.cpp
        test_update_sicd_version.cpp
        test_valid_six.cpp
        test_AMP8I_PHS8I.cpp
        test_utilities.cpp)

# Install the schemas
install(DIRECTORY "conf/schema/"
        DESTINATION "conf/schema/six/")

if (Python_Development_FOUND)
    # Install the Python schemas
    install(DIRECTORY "conf/schema/"
            DESTINATION "${CODA_PYTHON_SITE_PACKAGES}/pysix/schema/")
endif()

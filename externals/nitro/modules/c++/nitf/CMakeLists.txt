set(MODULE_NAME nitf)
set(MODULE_DEPS nitf-c j2k-c except-c++ io-c++ mt-c++ sys-c++ str-c++ gsl-c++ std-c++)
if (MSVC)
    list(APPEND MODULE_DEPS wsock32 ws2_32)
endif()

coda_add_module(
    ${MODULE_NAME}
    DEPS ${MODULE_DEPS}
    SOURCES
        source/BandInfo.cpp
        source/BandInfo.cpp
        source/BandSource.cpp
        source/BlockingInfo.cpp
        source/BufferedReader.cpp
        source/BufferedWriter.cpp
        source/ByteProvider.cpp
        source/ComponentInfo.cpp
        source/CompressedByteProvider.cpp
        source/CompressionInterface.cpp
        source/CustomIO.cpp
        source/DESegment.cpp
        source/DESubheader.cpp
        source/DataSource.cpp
        source/DateTime.cpp
        source/DecompressionInterface.cpp
        source/DownSampler.cpp
        source/Extensions.cpp
        source/Field.cpp
        source/FieldDescriptor.cpp
        source/FieldWarning.cpp
        source/FileHeader.cpp
        source/FileSecurity.cpp
        source/GraphicSegment.cpp
        source/GraphicSubheader.cpp
        source/Handle.cpp
        source/HashTable.cpp
        source/IOHandle.cpp
        source/IOInterface.cpp
        source/IOStreamReader.cpp
        source/IOStreamWriter.cpp
        source/ImageBlocker.cpp
        source/ImageReader.cpp
        source/ImageSegment.cpp
        source/ImageSegmentComputer.cpp
        source/ImageSource.cpp
        source/ImageSubheader.cpp
        source/ImageWriter.cpp
        source/J2KComponent.cpp
        source/J2KContainer.cpp
        source/J2KReader.cpp
        source/J2KWriter.cpp
        source/LabelSegment.cpp
        source/LabelSubheader.cpp
        source/List.cpp
        source/LookupTable.cpp
        source/MemoryIO.cpp
        source/NITFBufferList.cpp
        source/PluginRegistry.cpp
        source/RESegment.cpp
        source/RESubheader.cpp
        source/Reader.cpp
        source/Record.cpp
        source/SegmentReader.cpp
        source/SegmentSource.cpp
        source/SegmentWriter.cpp
        source/SubWindow.cpp
        source/TRE.cpp
        source/TestingTest.cpp
        source/TextSegment.cpp
        source/TextSubheader.cpp
        source/Utils.cpp
        source/WriteHandler.cpp
        source/Writer.cpp
        )

target_compile_features(nitf-c++ PUBLIC cxx_inheriting_constructors)

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "tests"
    DEPS cli-c++
    FILTER_LIST
        test_functional.cpp
        test_handles.cpp
        test_mem_source.cpp
        test_static_plugin.cpp
        test_j2k_nitf_read_region.cpp)

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "unittests"
    UNITTEST
    SOURCES
	test_create_nitf++.cpp
        test_field++.cpp
        test_image_blocker.cpp
        test_image_segment_blank_nm_compression.cpp
        test_image_segment_computer.cpp
        test_image_writer.cpp
        test_nitf_buffer_list.cpp
        test_tre_mods++.cpp
        test_tre_create++.cpp
        test_j2k_loading++.cpp
        test_j2k_read_tile.cpp
        test_tre_read.cpp)

add_executable(show_nitf++ apps/show_nitf++.cpp)
target_link_libraries(show_nitf++ PRIVATE nitf-c++)

install(TARGETS show_nitf++
        ${CODA_INSTALL_OPTION}
        RUNTIME DESTINATION "${CODA_STD_PROJECT_BIN_DIR}")

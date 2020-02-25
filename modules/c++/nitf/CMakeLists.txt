add_library(nitf-c++ source/BandInfo.cpp
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
                     source/TextSegment.cpp
                     source/TextSubheader.cpp
                     source/Utils.cpp
                     source/WriteHandler.cpp
                     source/Writer.cpp)

common_module_config(nitf-c++)
target_link_libraries(nitf-c++ nitf except-c++ io-c++ mt-c++ sys-c++ str-c++)
if (MSVC)
    target_link_libraries(nitf-c++ wsock32 ws2_32)
endif()
target_compile_features(nitf-c++ PUBLIC cxx_inheriting_constructors)

add_dependencies(nitf-c++ coda-oss nitf)

set(unittest_srcs test_field++
                  test_image_blocker
                  test_image_segment_blank_nm_compression
                  test_image_segment_computer
                  test_nitf_buffer_list
                  test_tre_mods++)

set(unittest_deps nitf-c++ nitf math-c++)
add_unittests("${unittest_srcs}" cpp "${unittest_deps}" nitf)

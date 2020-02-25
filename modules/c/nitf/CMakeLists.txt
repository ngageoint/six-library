configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/nitf/nitf_config.h.cmake.in
               ${CMAKE_CURRENT_SOURCE_DIR}/include/nitf/nitf_config.h)

add_library(nitf source/BandInfo.c
                 source/BandSource.c
                 source/ComplexityLevel.c
                 source/ComponentInfo.c
                 source/DESegment.c
                 source/DESubheader.c
                 source/DataSource.c
                 source/DefaultTRE.c
                 source/DirectBlockSource.c
                 source/DownSampler.c
                 source/Extensions.c
                 source/Field.c
                 source/FieldWarning.c
                 source/FileHeader.c
                 source/FileSecurity.c
                 source/GraphicSegment.c
                 source/GraphicSubheader.c
                 source/ImageIO.c
                 source/ImageReader.c
                 source/ImageSegment.c
                 source/ImageSource.c
                 source/ImageSubheader.c
                 source/ImageWriter.c
                 source/LabelSegment.c
                 source/LabelSubheader.c
                 source/LookupTable.c
                 source/PluginRegistry.c
                 source/RESegment.c
                 source/RESubheader.c
                 source/Reader.c
                 source/Record.c
                 source/RowSource.c
                 source/SegmentReader.c
                 source/SegmentSource.c
                 source/SegmentWriter.c
                 source/StreamIOWriteHandler.c
                 source/SubWindow.c
                 source/TRE.c
                 source/TRECursor.c
                 source/TREPrivateData.c
                 source/TREUtils.c
                 source/TextSegment.c
                 source/TextSubheader.c
                 source/WriteHandler.c
                 source/Writer.c)


common_module_config(nitf)

target_link_libraries(nitf nrt)
target_compile_definitions(nitf PRIVATE
                           NITF_DEFAULT_PLUGIN_PATH="${CMAKE_INSTALL_PREFIX}/share/nitf/plugins")


set(unittest_srcs test_create
                  test_create_nitf
                  test_field
                  test_image_io
                  test_mem_source
                  test_tre_mods
                  test_zero_field)

add_unittests("${unittest_srcs}" c nitf nitf)


# Build all the TRE
# TODO: --enable-static-tres
set(tre_srcs  ACCHZB
              ACCPOB
              ACCVTB
              ACFTA
              ACFTB
              AIMIDA
              AIMIDB
              AIPBCA
              ASTORA
              BANDSA
              BANDSB
              BCKGDA
              BLOCKA
              BNDPLB
              CCINFA
              CLCTNA
              CLCTNB
              CMETAA
              CSCCGA
              CSCRNA
              CSDIDA
              CSEPHA
              CSEXRA
              CSPROA
              CSSFAA
              CSSHPA
              ENGRDA
              EXOPTA
              EXPLTA
              EXPLTB
              GEOLOB
              GEOPSB
              GRDPSB
              HISTOA
              ICHIPB
              IMGDTA
              IOMAPA
              J2KLRA
              JITCID
              MAPLOB
              MENSRA
              MENSRB
              MPDSRA
              MSTGTA
              MTIRPA
              MTIRPB
              NBLOCA
              OBJCTA
              OFFSET
              PATCHA
              PATCHB
              PIAEQA
              PIAEVA
              PIAIMB
              PIAIMC
              PIAPEA
              PIAPEB
              PIAPRC
              PIAPRD
              PIATGA
              PIATGB
              PIXQLA
              PLTFMA
              PRJPSB
              REGPTB
              RPC00B
              RPFDES
              RPFHDR
              RPFIMG
              RSMAPA
              RSMDCA
              RSMECA
              RSMGGA
              RSMGIA
              RSMIDA
              RSMPCA
              RSMPIA
              SECTGA
              SENSRA
              SENSRB
              SNSPSB
              SNSRA
              SOURCB
              STDIDC
              STEROB
              STREOB
              TEST_DES
              TRGTA
              USE00A
              XML_DATA_CONTENT)

foreach(tre ${tre_srcs})
    add_library(${tre} SHARED shared/${tre}.c)
    target_link_libraries(${tre} nitf)

    # This line is making sure the resultant TRE is named, e.g.
    # XML_DATA_CONTENT.so instead of libXML_DATA_CONTENT.so
    set_target_properties(${tre} PROPERTIES PREFIX "")
endforeach()

install(TARGETS ${tre_srcs} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/nitf/plugins)

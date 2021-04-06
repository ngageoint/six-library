set(MODULE_NAME nitf)

coda_generate_module_config_header(${MODULE_NAME})

if (BUILD_SHARED)
    set(BUILD_SHARED_LIBS ON)
    add_definitions(
        -DNITF_MODULE_EXPORTS
    )
endif()

coda_add_module(
    ${MODULE_NAME}
    DEPS nrt-c
    SOURCES
        source/BandInfo.c
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
        source/NitfReader.c
        source/NitfSystem.c
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
        source/NitfWriter.c)

target_compile_definitions(nitf-c PRIVATE
                           NITF_DEFAULT_PLUGIN_PATH="${CMAKE_INSTALL_PREFIX}/share/nitf/plugins")

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "tests")

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "unittests"
    UNITTEST
    SOURCES
        test_create.c
        test_create_nitf.c
        test_field.c
        test_geo_utils.c
        test_image_io.c
        test_mem_source.c
        test_tre_mods.c
        test_zero_field.c
        test_moveTREs.c)


# Build all the TRE
set(ENABLE_STATIC_TRES OFF CACHE BOOL "Build static libraries for each TRE")
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
              CSEXRB
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
              IMASDA
              IMGDTA
              IMRFCA
              IOMAPA
              J2KLRA
              JITCID
              MAPLOB
              MATESA
              MENSRA
              MENSRB
              MPDSRA
              MSDIRA
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
              PIXMTA
              PIXQLA
              PLTFMA
              PRADAA
              PRJPSB
              PTPRAA
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
    target_link_libraries(${tre} PUBLIC nitf-c)
    target_compile_definitions(${tre} PRIVATE NITF_MODULE_EXPORTS)

    # This line is making sure the resultant TRE is named, e.g.
    # XML_DATA_CONTENT.so instead of libXML_DATA_CONTENT.so
    set_target_properties(${tre} PROPERTIES PREFIX "")

    if (ENABLE_STATIC_TRES)
        set(tre_static ${tre}-static-c)
        add_library(${tre_static} STATIC shared/${tre}.c)
        target_link_libraries(${tre_static} PUBLIC nitf-c)
        target_include_directories(${tre_static} PUBLIC
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
            "$<INSTALL_INTERFACE:include>")
        install(TARGETS ${tre_static}
            EXPORT ${CODA_EXPORT_SET_NAME}
            LIBRARY DESTINATION "lib"
            ARCHIVE DESTINATION "lib")
    endif()
endforeach()

install(TARGETS ${tre_srcs} DESTINATION "share/nitf/plugins")

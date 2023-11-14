set(MODULE_NAME j2k)

if (TARGET openjpeg)
    set(HAVE_OPENJPEG_H 1)
endif()

if (BUILD_SHARED)
    set(BUILD_SHARED_LIBS ON)
    add_definitions(
        -DJ2K_MODULE_EXPORTS
    )
endif()

coda_generate_module_config_header(${MODULE_NAME})

coda_add_module(
    ${MODULE_NAME}
    DEPS ${J2K_LIB} nitf-c nrt-c
    SOURCES
        source/Container.c
        source/Component.c
        source/Defines.c
        source/JasPerImpl.c
        source/OpenJPEGImpl.c
        source/j2k_Reader.c
        source/TileWriter.c
        source/SimpleComponentImpl.c
        source/SimpleContainerImpl.c
        source/j2k_Writer.c)

# Build all J2KCompress and J2KDecompress
set(j2k_shared_srcs  J2KCompress J2KDecompress)
foreach(j2k_shared ${j2k_shared_srcs})
    add_definitions(-DHAVE_J2K_H)

    add_library(${j2k_shared} SHARED shared/${j2k_shared}.c)
    target_link_libraries(${j2k_shared} PUBLIC nitf-c j2k-c)
    target_compile_definitions(${j2k_shared} PRIVATE NITF_MODULE_EXPORTS)

    # This line is making sure the resultant TRE is named, e.g.
    # XML_DATA_CONTENT.so instead of libXML_DATA_CONTENT.so
    set_target_properties(${j2k_shared} PROPERTIES PREFIX "")
endforeach()

install(TARGETS ${j2k_shared_srcs} DESTINATION "share/nitf/plugins")

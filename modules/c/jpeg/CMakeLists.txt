
if (BUILD_SHARED)
    set(BUILD_SHARED_LIBS ON)
    add_definitions(
        -DNITF_MODULE_EXPORTS
    )
endif()

if (TARGET jpeg)
    add_library(LibjpegDecompress SHARED source/LibjpegDecompress.c)
    target_link_libraries(LibjpegDecompress PUBLIC jpeg nitf-c)
    set_target_properties(LibjpegDecompress PROPERTIES PREFIX "")
    install(TARGETS LibjpegDecompress DESTINATION "share/nitf/plugins")
endif()

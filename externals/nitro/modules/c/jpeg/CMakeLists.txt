if (TARGET jpeg) # refers to libjpeg (external dependency)
    coda_add_module(
        jpeg    # becomes jpeg-c (nitro module)
        DEPS jpeg nitf-c
        SOURCES source/LibjpegDecompress.c)
endif()

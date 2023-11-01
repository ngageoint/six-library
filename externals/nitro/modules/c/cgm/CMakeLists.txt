set(MODULE_NAME cgm)

if (BUILD_SHARED)
    set(BUILD_SHARED_LIBS ON)
    add_definitions(
        -DNITF_MODULE_EXPORTS
    )
endif()

coda_add_module(
    ${MODULE_NAME}
    DEPS nitf-c
    SOURCES
        source/CircleElement.c
        source/CircularArcCloseElement.c
        source/CircularArcElement.c
        source/Color.c
        source/Element.c
        source/EllipseElement.c
        source/EllipticalArcCloseElement.c
        source/EllipticalArcElement.c
        source/FillAttributes.c
        source/LineAttributes.c
        source/Metafile.c
        source/MetafileReader.c
        source/MetafileWriter.c
        source/NITFWriteHandler.c
        source/Picture.c
        source/PictureBody.c
        source/PolyLineElement.c
        source/PolySetElement.c
        source/PolygonElement.c
        source/Rectangle.c
        source/RectangleElement.c
        source/Text.c
        source/TextAttributes.c
        source/TextElement.c
        source/Vertex.c
        source/VertexClose.c)


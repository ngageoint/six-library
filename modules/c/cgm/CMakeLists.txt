add_library(cgm source/CircleElement.c
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
common_module_config(cgm)
target_link_libraries(cgm nitf-c)

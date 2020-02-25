add_library(JPEG STATIC IMPORTED)
set_target_properties(JPEG PROPERTIES
    IMPORTED_LOCATION ${CODA_INSTALL_DIR}/lib/liblibjpeg.a
    INTERFACE_INCLUDE_DIRECTORIES ${CODA_INSTALL_DIR}/include)

add_library(jpeg-c source/LibjpegDecompress.c)
add_dependencies(jpeg-c coda-oss)
target_link_libraries(jpeg-c JPEG nitf)
target_include_directories(jpeg-c PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
install(TARGETS jpeg-c)

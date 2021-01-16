set(java_files ${CMAKE_CURRENT_SOURCE_DIR}/src/java/nitf/imagej/NITF_Reader.java)

add_jar(nitf.imagej-java "${java_files}"
        INCLUDE_JARS ${coda_jars}
                     ${CMAKE_CURRENT_BINARY_DIR}/../nitf/nitf.jar
                     ${CMAKE_CURRENT_BINARY_DIR}/../nitf.imageio/nitf.imageio.jar
                     ${CMAKE_CURRENT_SOURCE_DIR}/lib/ij-1.42.jar
        OUTPUT_NAME nitf.imagej)

add_dependencies(nitf.imagej-java nitf-java nitf.imageio-java)
install_jar(nitf.imagej-java ${CMAKE_INSTALL_PREFIX}/lib)

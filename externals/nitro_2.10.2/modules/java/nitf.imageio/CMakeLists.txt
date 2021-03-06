set(java_source_dir ${CMAKE_CURRENT_SOURCE_DIR}/src/java/nitf/imageio)
set(java_files ${java_source_dir}/ImageIOUtils.java
               ${java_source_dir}/NITFReader.java
               ${java_source_dir}/NITFReaderSpi.java)


add_jar(nitf.imageio-java "${java_files}"
        INCLUDE_JARS ${coda_jars}
        ${CMAKE_CURRENT_BINARY_DIR}/../nitf/nitf.jar
        OUTPUT_NAME nitf.imageio)

add_dependencies(nitf.imageio-java nitf-java)
install_jar(nitf.imageio-java ${CMAKE_INSTALL_PREFIX}/lib)

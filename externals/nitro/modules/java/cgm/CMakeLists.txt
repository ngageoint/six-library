set(cgm_source_dir ${CMAKE_CURRENT_SOURCE_DIR}/src/java/cgm/)
set(java_files ${cgm_source_dir}/CGMUtils.java
               ${cgm_source_dir}/CircleElement.java
               ${cgm_source_dir}/Element.java
               ${cgm_source_dir}/Metafile.java
               ${cgm_source_dir}/MetafileReader.java
               ${cgm_source_dir}/PictureBody.java
               ${cgm_source_dir}/Picture.java
               ${cgm_source_dir}/Rectangle.java
               ${cgm_source_dir}/TextAttributes.java
               ${cgm_source_dir}/TextElement.java
               ${cgm_source_dir}/Text.java)

set(cgm_jni_source_dir ${CMAKE_CURRENT_SOURCE_DIR}/src/jni/source/)
set(c_files ${cgm_jni_source_dir}/cgm_Element.c
            ${cgm_jni_source_dir}/cgm_JNI.c
            ${cgm_jni_source_dir}/cgm_Metafile.c
            ${cgm_jni_source_dir}/cgm_MetafileReader.c
            ${cgm_jni_source_dir}/cgm_Picture.c)

add_java_module(cgm "${java_files}" "${c_files}"
                ${CMAKE_CURRENT_BINARY_DIR}/../nitf/nitf.jar)
add_dependencies(cgm-java nitf-java cgm-c)

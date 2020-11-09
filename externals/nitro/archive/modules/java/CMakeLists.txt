find_package(JNI)
find_package(Java)
include(UseJava)

function(add_java_module target java_files c_files jars)
    add_jar(${target}-java ${java_files}
        INCLUDE_JARS ${jars}
        OUTPUT_NAME ${target})

    get_target_property(jar_file ${target}-java JAR_FILE)
    get_target_property(class_dir ${target}-java CLASSDIR)

    include_directories(${JNI_INCLUDE_DIRS}
                        ${class_dir}
                        ${CMAKE_CURRENT_BINARY_DIR}
                        ${CMAKE_CURRENT_SOURCE_DIR}/src/jni/include/)

    add_library(${target}-jni MODULE ${c_files})
    set_target_properties(${target}-jni PROPERTIES SUFFIX .jni-c.so)
    set_target_properties(${target}-jni PROPERTIES OUTPUT_NAME ${target})
    target_link_libraries(${target}-jni ${JNI_LIBRARIES} nrt-c nitf-c cgm-c)
    install(TARGETS ${target}-jni DESTINATION lib)
    install(FILES ${jar_file} DESTINATION lib)
endfunction()

if (NOT JNI_FOUND)
    message(WARNING "JNI libraries not found. Skipping Java modules")
elseif(NOT Java_FOUND)
    message(WARNING "Java SDK not found. Skipping Java modules")
else()
    set(CMAKE_JAVA_COMPILE_FLAGS "-source" "1.8" "-target" "1.8" "-Xlint:deprecation" "-Xlint:-options")

    set(coda_jars "${CODA-OSS_JARS_DIR}/commons-io-1.3.2.jar"
                  "${CODA-OSS_JARS_DIR}/commons-cli-1.0-beta-2.jar"
                  "${CODA-OSS_JARS_DIR}/commons-lang-2.0.jar"
                  "${CODA-OSS_JARS_DIR}/commons-logging-1.1.1.jar"
                  "${CODA-OSS_JARS_DIR}/log4j-1.2.13.jar")

    add_subdirectory(nitf)
    add_subdirectory(cgm)
    add_subdirectory(nitf.imageio)
    add_subdirectory(nitf.imagej)
endif()

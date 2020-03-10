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
    install(TARGETS ${target}-jni DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
    install(FILES ${jar_file} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
endfunction()

if (NOT JNI_FOUND)
    message(WARNING "JNI libraries not found. Skipping Java modules")
elseif(NOT Java_FOUND)
    message(WARNING "Java SDK not found. Skipping Java modules")
else()
    set(CMAKE_JAVA_COMPILE_FLAGS "-source" "1.5" "-target" "1.5")

    # CODA doesn't know how to install jars, so we'll handle it here
    set(jar_path ${CODA_DIR}/modules/drivers/jars)
    set(coda_jars ${jar_path}/commons-io-1.3.2.jar
                  ${jar_path}/commons-cli-1.0-beta-2.jar
                  ${jar_path}/commons-lang-2.0.jar
                  ${jar_path}/commons-logging-1.1.1.jar
                  ${jar_path}/log4j-1.2.13.jar)
    install(FILES ${coda_jars} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

    add_subdirectory(nitf)
    add_subdirectory(cgm)
    add_subdirectory(nitf.imageio)
    add_subdirectory(nitf.imagej)
endif()

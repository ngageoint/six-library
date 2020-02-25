add_library(J2K STATIC IMPORTED)
set_target_properties(J2K PROPERTIES
    IMPORTED_LOCATION ${CODA_INSTALL_DIR}/lib/libopenjpeg.a
    INTERFACE_INCLUDE_DIRECTORIES ${CODA_INSTALL_DIR}/include)
target_link_libraries(J2K INTERFACE m)

add_library(j2k_config INTERFACE)
target_include_directories(j2k_config INTERFACE ${CODA_DIR}/target/modules/drivers/j2k/include/)

add_library(j2k-c source/Container.c
                  source/Component.c
                  source/JasPerImpl.c
                  source/OpenJPEGImpl.c
                  source/Reader.c
                  source/SimpleComponentImpl.c
                  source/SimpleContainerImpl.c
                  source/Writer.c)

common_module_config(j2k-c)
add_dependencies(j2k-c coda-oss)

target_link_libraries(j2k-c j2k_config J2K nrt nitf)

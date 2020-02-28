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

target_link_libraries(j2k-c j2k_config J2K nitf-c)

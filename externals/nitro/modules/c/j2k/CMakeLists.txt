set(MODULE_NAME j2k)

if (TARGET openjpeg)
    set(HAVE_OPENJPEG_H 1)
endif()

coda_generate_module_config_header(${MODULE_NAME})

coda_add_module(
    ${MODULE_NAME}
    DEPS ${J2K_LIB} nitf-c nrt-c
    SOURCES
        source/Container.c
        source/Component.c
        source/JasPerImpl.c
        source/OpenJPEGImpl.c
        source/j2k_Reader.c
        source/SimpleComponentImpl.c
        source/SimpleContainerImpl.c
        source/j2k_Writer.c)

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "tests"
    SOURCES
        test_j2k_create.c
        test_j2k_header.c
        test_j2k_nitf.c
        test_j2k_read_region.c
        test_j2k_read_tile.c)

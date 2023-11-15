set(MODULE_NAME hdf5)
set(TARGET_LANGUAGE c++)

# set up warnings
if (MSVC)
    # DO NOT use standard (not legacy) C pre-processor
    add_compile_options(/Zc:preprocessor-)

    add_compile_definitions (_BIND_TO_CURRENT_VCLIBS_VERSION=1 _CONSOLE)
endif()

coda_add_module(${MODULE_NAME} VERSION 1.14.2)


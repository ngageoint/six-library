set(MODULE_NAME hdf5)
set(TARGET_LANGUAGE c++)

if(CODA_ENABLE_HDF5)
    # set up warnings
    if (MSVC)
        # DO NOT use standard (not legacy) C pre-processor
        add_compile_options(/Zc:preprocessor-)
    endif()

    coda_add_module(${MODULE_NAME} VERSION 1.13.2)
else()
    message("${MODULE_NAME} will not be built since HDF5 is not enabled")
endif()

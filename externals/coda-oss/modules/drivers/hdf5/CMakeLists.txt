set(MODULE_NAME hdf5)
set(TARGET_LANGUAGE c++)

if(CODA_ENABLE_HDF5)
    # set up warnings
    if (MSVC)
        add_compile_definitions(_BIND_TO_CURRENT_VCLIBS_VERSION=1 _CONSOLE)
    endif()
    add_compile_options(
        $<$<CXX_COMPILER_ID:MSVC>:/Zc:preprocessor->
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wno-implicit-function-declaration>
    )

    coda_add_module(${MODULE_NAME} VERSION 1.14.2)
else()
    message("${MODULE_NAME} will not be built since HDF5 is not enabled")
endif()

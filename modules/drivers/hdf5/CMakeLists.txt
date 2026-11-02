set(MODULE_NAME hdf5)
set(TARGET_LANGUAGE c++)

if(CODA_ENABLE_HDF5)

    if(MSVC)
        add_compile_options(/wd4005)
        add_compile_options(/wd4244)
        add_compile_options(/wd4319)
        add_compile_options(/wd4018)
        add_compile_options(/wd4101)
        add_compile_options(/wd4090)
        add_compile_options(/wd4267)
    endif()

    set(BUILD_SHARED_LIBS OFF)
    set(BUILD_TESTING OFF)
    set(HDF5_BUILD_EXAMPLES OFF)
    set(HDF5_BUILD_CPP_LIB ON)

    FetchContent_Declare(hdf5
        URL "${CMAKE_CURRENT_SOURCE_DIR}/hdf5-2.2.0.tar.gz"
        URL_HASH "SHA256=1a1ab8209b35586fbc1aa279ba76d102130b95badcb20ca329587219112d8c16")
    
    FetchContent_MakeAvailable(hdf5)

    coda_add_module(${MODULE_NAME} VERSION 2.2.0 DEPS hdf5-static hdf5_cpp-static)
    install(TARGETS hdf5-static hdf5_cpp-static EXPORT ${CODA_EXPORT_SET_NAME} ${CODA_INSTALL_OPTION})

else()
    message("${MODULE_NAME} will not be built since HDF5 is not enabled")
endif()

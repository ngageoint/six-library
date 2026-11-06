set(MODULE_NAME highfive)
set(TARGET_LANGUAGE c++)

if(CODA_ENABLE_HDF5)
    set(HIGHFIVE_FIND_HDF5 OFF)
    FetchContent_Declare(highfive
        URL "${CMAKE_CURRENT_SOURCE_DIR}/highfive-3.3.0.tar.gz"
        URL_HASH "SHA256=325cfbcf0c0296a6dd26f3b088801b7ebb8d6f109c0565c11d2d8c4af3253bff")
    
    FetchContent_MakeAvailable(highfive)

    coda_add_module(
        ${MODULE_NAME}
        VERSION 3.3.0
        DEPS HighFive::HighFive hdf5-c++)
        
else()
    message("${MODULE_NAME} will not be built since HDF5 is not enabled")
endif()

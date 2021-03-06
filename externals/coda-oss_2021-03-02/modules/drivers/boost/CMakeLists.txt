set(config_file_out "include/boost/boost_config.h")
configure_file("include/boost/boost_config.h.in" "${config_file_out}" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${config_file_out}"
        DESTINATION "include/boost"
        ${CODA_INSTALL_OPTION})
if (Boost_serialization_FOUND)
    get_property(inc_dirs TARGET Boost::serialization PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
    list(APPEND inc_dirs
         "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
         "$<INSTALL_INTERFACE:${CODA_STD_PROJECT_INCLUDE_DIR}>")
    set_property(TARGET Boost::serialization PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                 ${inc_dirs})
endif()

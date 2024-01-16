set(MODULE_NAME config)
set(TARGET_NAME ${MODULE_NAME}-c++)

if (WIN32)
    set(SOCKET_LIB ws2_32)
endif()

coda_add_module(${MODULE_NAME}
                VERSION 1.2
                DEPS Threads::Threads ${CMAKE_DL_LIBS} ${CLOCK_GETTIME_EXTRALIBS} ${SOCKET_LIB})

set(config_file_out "include/config/coda_oss_config.h")
configure_file("include/config/coda_oss_config.h.in" "${config_file_out}" @ONLY)
target_include_directories(${TARGET_NAME} INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/${CODA_STD_PROJECT_INCLUDE_DIR}>"
    "$<INSTALL_INTERFACE:${CODA_STD_PROJECT_INCLUDE_DIR}>")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${config_file_out}"
        DESTINATION "include/config"
        ${CODA_INSTALL_OPTION})

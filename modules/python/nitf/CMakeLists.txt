cmake_policy(SET CMP0078 NEW)
cmake_policy(SET CMP0086 NEW)
include(UseSWIG)
if (Python_FOUND)
    set_property(SOURCE source/nitro.i PROPERTY INCLUDE_DIRECTORIES
        # Include from binary dir for generated config sources
        "${CMAKE_CURRENT_BINARY_DIR}/../../c/nrt/include"
        "${CMAKE_CURRENT_BINARY_DIR}/../../c/nitf/include"

        # And then manually add source includes for everything else we need
        # TODO: Is there a better way to do this...?
        "${NITRO_C_SOURCE_DIR}/nrt/include"
        "${NITRO_C_SOURCE_DIR}/nitf/include")
    set_property(SOURCE source/nitro.i PROPERTY CPLUSPLUS ON)
    swig_add_library(nitropy TYPE SHARED
                             LANGUAGE python
                             OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source/generated
                             OUTFILE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source/generated
                             SOURCES source/nitro.i)

    target_include_directories(nitropy PRIVATE ${Python_INCLUDE_DIRS} ${Python_NumPy_INCLUDE_DIRS}
                                               ${NITRO_C_SOURCE_DIR}/nrt/include
                                               ${NITRO_C_SOURCE_DIR}/nitf/include)
    target_link_libraries(nitropy nitf numpyutils-c++ sys-c++ except-c++ io-c++ str-c++)
    install(TARGETS nitropy DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/site-packages/)
    install(FILES source/__init__.py source/generated/nitropy.py
            DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/site-packages/)
    add_dependencies(nitropy coda-oss)
else()
    message(FATAL_ERROR "Can't find PYTHON_HOME")
endif()

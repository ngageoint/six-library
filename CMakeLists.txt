cmake_minimum_required(VERSION 3.11)
project(six-library)

include(externals/nitro/common_build.cmake)
enable_testing()

find_package(Python COMPONENTS Interpreter Development Numpy)
if (Python_FOUND)
    set(PYTHON ${PYTHON_EXECUTABLE})
else()
    set(PYTHON python)
endif()


list(APPEND coda_static_modules mt-c++ math.linear-c++ mem-c++ except-c++ sys-c++
logging-c++ str-c++ math-c++ io-c++ types-c++ polygon-c++ xml.lite-c++ tiff-c++ J2K)
list(APPEND coda_header_modules math.poly-c++)
list(APPEND coda_modules ${coda_static_modules} ${coda_header_modules} XML)
set(CODA_INSTALL_DIR ${CMAKE_INSTALL_PREFIX})
include(CMakeLists-NITRO.txt)

foreach(module ${coda_static_modules})
    add_coda_module(${module})
endforeach()
foreach(module ${coda_header_modules})
    add_library(${module} INTERFACE)
    target_include_directories(${module} INTERFACE ${CODA_INSTALL_DIR}/include)
endforeach()

add_coda_module(nitf-c++)
add_coda_module(nitf-c)
add_coda_module(nrt-c)
target_link_libraries(nitf-c++ INTERFACE nitf-c nrt-c)
target_link_libraries(xml.lite-c++ INTERFACE XML)

add_library(XML_DATA_CONTENT-static-c STATIC IMPORTED)
set_target_properties(XML_DATA_CONTENT-static-c PROPERTIES IMPORTED_LOCATION
                      ${CODA_INSTALL_DIR}/lib/XML_DATA_CONTENT-static-c.a)

add_library(XML STATIC IMPORTED)
if(UNIX)
    set_target_properties(XML PROPERTIES IMPORTED_LOCATION
                          ${CODA_INSTALL_DIR}/lib/libxerces-c.a)
elseif(MSVC)
    set_target_properties(XML PROPERTIES IMPORTED_LOCATION
                          ${CODA_INSTALL_DIR}/lib/xerces-c.lib)
endif()

include_directories(${CODA_INSTALL_DIR}/include)

add_subdirectory(six)

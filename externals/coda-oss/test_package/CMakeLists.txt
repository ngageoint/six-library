cmake_minimum_required(VERSION 3.14)
project(coda-oss-test_package)
set(CMAKE_CXX_STANDARD 11)
include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
find_package(coda-oss)
include(CodaBuild)

if (MSVC)
    coda_setup_msvc_crt()
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
foreach (build_type "RELEASE" "DEBUG" "RELWITHDEBINFO" "MINSIZEREL")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${build_type} ${CMAKE_CURRENT_BINARY_DIR}/bin)
endforeach()

add_executable(test test.cpp)
target_link_libraries(test types-c++ str-c++)

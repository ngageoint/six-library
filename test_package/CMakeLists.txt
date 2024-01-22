cmake_minimum_required(VERSION 3.14)
project(nitro-test_package)
set(CMAKE_CXX_STANDARD 14)
set(CXX_STANDARD_REQUIRED true)
include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
find_package(nitro)

foreach (suffix "" "_RELEASE" "_DEBUG" "_RELWITHDEBINFO" "_MINSIZEREL")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY${suffix} ${CMAKE_CURRENT_BINARY_DIR}/bin)
endforeach()

add_definitions(-DNOMINMAX)

add_executable(test test.cpp)
target_link_libraries(test nitf-c++)

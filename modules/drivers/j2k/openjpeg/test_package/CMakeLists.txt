cmake_minimum_required(VERSION 3.14)
project(coda-oss_openjpeg-test_package)
set(CMAKE_CXX_STANDARD 11)

include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
conan_basic_setup()

# import targets
include("${CMAKE_BINARY_DIR}/conan_paths.cmake")
find_package(coda-oss_openjpeg)

add_executable(test test.cpp)
target_link_libraries(test openjpeg)

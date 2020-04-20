cmake_minimum_required(VERSION 3.14)
project(six-library-test_package)
set(CMAKE_CXX_STANDARD 11)
include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
find_package(six-library REQUIRED)

foreach (suffix "" "_RELEASE" "_DEBUG" "_RELWITHDEBINFO" "_MINSIZEREL")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY${suffix} ${CMAKE_CURRENT_BINARY_DIR}/bin)
endforeach()

add_definitions(-DNOMINMAX -D_USE_MATH_DEFINES)

add_executable(test test.cpp)
target_link_libraries(test six.sicd-c++)

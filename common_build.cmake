set(CMAKE_C_STANDARD 90)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Select Build Type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    message("Overriding default CMAKE_INSTALL_PREFIX of ${CMAKE_INSTALL_PREFIX}")
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install${CMAKE_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}" CACHE PATH "Install directory" FORCE)
endif()

if (UNIX)
    add_compile_options(-Wno-deprecated -Wno-deprecated-declarations)
elseif(MSVC)
    add_definitions(-DWIN32_LEAN_AND_MEAN
                    -DNOMINMAX
                    -D_CRT_SECURE_NO_WARNINGS
                    -D_SCL_SECURE_NO_WARNINGS
                    -D_USE_MATH_DEFINES)
endif()

function(common_module_config module)
    target_include_directories(${module} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
                                                ${CMAKE_CURRENT_BINARY_DIR}/include)
    install(TARGETS ${module})
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
            DESTINATION include)
endfunction()

function(add_unittests unittests ext deps module)
    foreach(TEST_NAME ${unittests})
        add_executable(${TEST_NAME} unittests/${TEST_NAME}.${ext})
        target_link_libraries(${TEST_NAME} ${deps})
        add_test(${TEST_NAME} ${TEST_NAME})
        install(TARGETS ${TEST_NAME} DESTINATION unittests/${module})
    endforeach()
endfunction()

function(add_coda_module module)
    add_library(${module} STATIC IMPORTED)
    if(UNIX)
        set_target_properties(${module} PROPERTIES
            IMPORTED_LOCATION ${CODA_INSTALL_DIR}/lib/lib${module}.a)
    elseif(MSVC)
        set_target_properties(${module} PROPERTIES
            IMPORTED_LOCATION ${CODA_INSTALL_DIR}/lib/${module}.lib)
    else()
        message(FATAL_ERROR "Unsupported platform")
    endif()
endfunction()


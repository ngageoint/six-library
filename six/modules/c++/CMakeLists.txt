set(TARGET_LANGUAGE c++)

# be sure we're really using C++17
if (MSVC)
    add_compile_options(/std:c++17)
elseif (UNIX)
    add_compile_options(-std=c++17)
endif()

add_subdirectory(scene)
add_subdirectory(six)
add_subdirectory(six.sicd)
add_subdirectory(six.sidd)
add_subdirectory(cphd)
add_subdirectory(cphd03)
add_subdirectory(six.convert)
add_subdirectory(samples)

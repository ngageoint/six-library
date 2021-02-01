set(TARGET_LANGUAGE c++)

# be sure we're really using C++17
if (MSVC)
    add_compile_options(/std:c++17)
elseif (UNIX)
    add_compile_options(-std=c++17)
endif()

add_subdirectory(nitf)

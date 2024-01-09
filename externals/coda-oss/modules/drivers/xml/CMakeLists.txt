if (MSVC)
    add_compile_options(/wd4996) # '...': was declared deprecated
    add_compile_options(/wd4996) # '...': The POSIX name for this item is deprecated. Instead, use the ISO C and C++ conformant name: .... See online help for details.
    add_compile_options(/wd4005) # '...': macro redefinition
endif()

add_subdirectory("xerces")


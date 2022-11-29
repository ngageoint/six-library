set(MODULE_NAME numpyutils)

if(Python_Development_FOUND AND Python_NumPy_FOUND)
    set(MODULE_DEPS types-c++ except-c++ sys-c++ ${Python_LIBRARIES})
    if (UNIX)
        list(APPEND MODULE_DEPS util)
    endif()
    if (MSVC AND MSVC_VERSION LESS 1920)
        add_compile_options(
            /wd4505 # disable warning 'unreferenced local function has been removed'
            /wd4551 # function call missing argument list
        )
    endif()
    coda_add_module(
        ${MODULE_NAME}
        VERSION 1.0
        DEPS ${MODULE_DEPS})
    target_include_directories(${MODULE_NAME}-c++ PUBLIC
        ${Python_INCLUDE_DIRS} ${Python_NumPy_INCLUDE_DIRS})

    coda_add_tests(
        MODULE_NAME ${MODULE_NAME}
        DIRECTORY "unittests"
        UNITTEST)
endif()

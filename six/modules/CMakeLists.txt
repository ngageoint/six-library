set(TARGET_LANGUAGE c++)

add_subdirectory(c++)
if (BUILD_PYTHON_MODULES)
    add_subdirectory(python)
endif()

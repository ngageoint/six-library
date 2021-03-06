set(MODULE_NAME re)
set(MODULE_DEPS sys-c++)

# Enable to use std::regex instead of PCRE.
set(RE_ENABLE_STD_REGEX OFF CACHE BOOL "use std::regex instead of pcre")

if (RE_ENABLE_STD_REGEX OR TARGET pcre2)
    coda_generate_module_config_header(${MODULE_NAME})

    if (NOT RE_ENABLE_STD_REGEX)
        list(APPEND MODULE_DEPS pcre2)
    endif()

    coda_add_module(
        ${MODULE_NAME}
        VERSION 1.0
        DEPS ${MODULE_DEPS})

    if (NOT RE_ENABLE_STD_REGEX)
        if (NOT BUILD_SHARED_LIBS)
            # this definition is required to statically link against pcre2
            # see NON_AUTOTOOLS_BUILD, LINKING PROGRAMS IN WINDOWS ENVIRONMENTS section
            target_compile_definitions(${MODULE_NAME}-c++ PUBLIC -DPCRE2_STATIC)
        endif()
    endif()

    coda_add_tests(
        MODULE_NAME ${MODULE_NAME}
        DIRECTORY "tests")
    coda_add_tests(
        MODULE_NAME ${MODULE_NAME}
        DIRECTORY "unittests"
        UNITTEST)
endif()

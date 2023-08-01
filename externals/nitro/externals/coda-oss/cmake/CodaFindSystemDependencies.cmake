# find and import system dependencies
macro(coda_find_system_dependencies)
    # creates imported target Threads::Threads
    # see https://cmake.org/cmake/help/latest/module/FindThreads.html
    set(THREADS_PREFER_PTHREAD_FLAG TRUE)
    find_package(Threads)

    # creates imported target CURL::libcurl, if found
    # see https://cmake.org/cmake/help/latest/module/FindCURL.html
    find_package(CURL)

    # creates imported target Boost::serialization, if found
    # see https://cmake.org/cmake/help/latest/module/FindBoost.html
    set(ENABLE_BOOST OFF CACHE BOOL "Enable building modules dependent on Boost")

    # sets the following variables if Python installation found:
    #   Python_FOUND                - flag indicating system has the requested components
    #   Python_VERSION
    #   Python_VERSION_MAJOR
    #   Python_VERSION_MINOR
    #   Python_VERSION_PATCH
    #
    #   Python_Interpreter_FOUND    - flag indicating system has the interpreter
    #   Python_EXECUTABLE           - path to interpreter
    #   Python_STDLIB               - standard installation directory
    #
    #   Python_Development_FOUND    - flag indicating system has development artifacts
    #   Python_INCLUDE_DIRS         - Python include directories
    #   Python_LIBRARIES            - Python libraries
    #
    #   Python_NumPy_FOUND          - flag indicating system has NumPy
    #   Python_NumPy_INCLUDE_DIRS   - NumPy include directories
    #
    # see https://cmake.org/cmake/help/latest/module/FindPython.html
    set(ENABLE_PYTHON ON CACHE BOOL "Enable building Python modules")
    set(PYTHON_VERSION "" CACHE STRING "Hint for which version of Python to find")
    set(PYTHON_HOME "" CACHE PATH "Path to existing Python installation")
    set(ENABLE_SWIG OFF CACHE BOOL "Enable generation of SWIG bindings")
    if (PYTHON_HOME)
        # specifying PYTHON_HOME implies ENABLE_PYTHON
        set(ENABLE_PYTHON ON CACHE BOOL "Enable building Python modules" FORCE)
    endif()
    if (ENABLE_PYTHON OR PYTHON_HOME)
        set(Python_FIND_STRATEGY LOCATION)
        if (PYTHON_HOME)
            set(Python_ROOT_DIR ${PYTHON_HOME})
            set(Python_FIND_VIRTUALENV STANDARD)
            set(Python_FIND_REGISTRY NEVER)
        else()
            set(Python_FIND_VIRTUALENV FIRST)
            set(Python_FIND_REGISTRY LAST)
        endif()
        find_package(Python ${PYTHON_VERSION}
                     COMPONENTS Interpreter Development NumPy)
        if (Python_Development_FOUND)
            set(CODA_PYTHON_SITE_PACKAGES
                "${CODA_STD_PROJECT_LIB_DIR}/python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}/site-packages")
            if(NOT PYTHON_HOME)
                message("Python installation found at ${Python_EXECUTABLE}.\n"
                        "Pass the configure options -DPYTHON_HOME=... or "
                        "-DPYTHON_VERSION=... to override this selection.")
            endif()
            if (Python_Interpreter_FOUND)
                message("Python_EXECUTABLE=${Python_EXECUTABLE}")
            endif()
            if (Python_Development_FOUND)
                message("Python_INCLUDE_DIRS=${Python_INCLUDE_DIRS}")
                message("Python_LIBRARIES=${Python_LIBRARIES}")
            endif()
            if (Python_NumPy_FOUND)
                message("Python_NumPy_INCLUDE_DIRS=${Python_NumPy_INCLUDE_DIRS}")
            endif()

            set(BUILD_PYTHON_MODULES ON CACHE INTERNAL "Whether Python modules will be built")
            if (MSVC)
                message("Note: MSVC builds of Python modules linked to a "
                        "MSVC debug runtime library (Debug builds) require that "
                        "Python is installed with debug symbols and binaries. "
                        "This can be done with the python.org installer but is "
                        "currently (April 2020) not supported by Anaconda.")
            endif()

            if (ENABLE_SWIG)
                # if SWIG found, sets path to swig utilities in SWIG_USE_FILE
                # see https://cmake.org/cmake/help/latest/module/UseSWIG.html
                find_package(SWIG 3.0)
                if (SWIG_FOUND)
                    cmake_policy(SET CMP0078 NEW) # UseSWIG generates standard target names
                    cmake_policy(SET CMP0086 NEW) # UseSWIG honors SWIG_MODULE_NAME via -module
                    include(${SWIG_USE_FILE})
                    option(PYTHON_EXTRA_NATIVE "generate extra native containers with SWIG" ON)
                    if (PYTHON_EXTRA_NATIVE)
                        list(APPEND CMAKE_SWIG_FLAGS "-extranative")
                    endif()
                endif()
            else()
                set(SWIG_FOUND OFF CACHE INTERNAL "")
            endif()
        else()
            message(FATAL_ERROR "Python targets will not be built since Python "
                    "development artifacts were not found. Pass the configure "
                    "option -DPYTHON_HOME=... or build inside a virtualenv to \
                    help locate an installation, or set ENABLE_PYTHON=OFF")
        endif()
    endif()

    # sets OPENSSL_FOUND to TRUE if found, and creates targets
    # OpenSSL:SSL and OpenSSL::Crypto
    # see https://cmake.org/cmake/help/latest/module/FindOpenSSL.html
    find_package(OpenSSL)
endmacro()

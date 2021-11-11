# CMake build system for CODA-oss #

August 2018, Scott.Colcord@maxar.com <br />
April 2020, Keith.Wilhelm@maxar.com

CODA-oss now contains a CMake-based build system.

## Dependencies ##
### Required ###
* CMake >= 3.14, configured with "--qt-gui" if the qt gui is desired.
### Optional ###
* Python (help locate by passing `-DPYTHON_HOME=[path]`)
* SWIG (help locate by adding binary to the PATH)
* Boost (help locate by passing `-DBOOST_HOME=[path]`)
* curl

## Files ##
* CMakeLists.txt - Entry point for the CMake build.
* cmake/CodaBuild.cmake - CMake build tools
* cmake/FindSystemDependencies.cmake - Tools for finding system dependencies, needed by consumers
* modules/.../CMakeLists.txt - Project specific options and settings.
* modules/.../include/.../*.cmake.in - Templates for generating config header files.

## How to build ##
### Instructions for Linux ###
```
# starting from base directory of repo, make a build directory and cd into it
mkdir target
cd target

# configure
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=[install path]

# build and install, in parallel
cmake --build . --target install -- -j

# run unit tests (optional)
ctest
```

To select the compilers to use, either set the environment variables `CC=[path]/gcc CXX=[path]/g++` or pass the configure options `-DCMAKE_C_COMPILER=[path]/gcc -DCMAKE_CXX_COMPILER=[path]/g++`

### Instructions for Windows ###
From command or bash prompt:
```
# starting from base directory of repo, make a build directory and cd into it
mkdir target
cd target

# configure
cmake .. -DCMAKE_INSTALL_PREFIX=[install path]

# build and install, in parallel
# unlike Linux, build type is specified here instead of during the configuration step
cmake --build . --config Release --target install -j

# run unit tests (optional)
ctest -C Release
```

Or open the directory containing the top-level CMakeLists.txt as a project in Visual Studio.

To select the compiler and architecture, pass the -G and -A options to cmake in the configure step. For example:

`-G "Visual Studio 14 2015" -A x64` (Visual Studio 2015 64-bit build) <br />
`-G "Visual Studio 15 2017" -A Win32` (Visual Studio 2017 32-bit build)


## Build configuration ##
These options may be passed in the cmake configure step as `-DOPTION_NAME="option value"`
| Option Name    | Default Value | Description |
|----------------|---------------|-------------|
|CMAKE_BUILD_TYPE|RelWithDebInfo|build type (Release, Debug, RelWithDebInfo); not used for MSVC builds (should be specified at build time instead)|
|BUILD_SHARED_LIBS|OFF|build shared libraries if on, static if off (note: not working on Windows)|
|STATIC_CRT|OFF|for Windows MSVC builds only, link with /MT (or /MTd for Debug builds) if on, or with /MD (or /MDd for Debug builds) if off|
|CODA_BUILD_TESTS| ON      |build tests if on|
|CODA_PARTIAL_INSTALL|OFF|make the install target not depend on all defined targets, only the targets which have already been built will be installed; cmake/CodaBuild.cmake for further information and caveats|
|MT_DEFAULT_PINNING|OFF|use affinity-based CPU pinning by default in MT|
|ENABLE_BOOST|OFF|build modules dependent on Boost if enabled|
|ENABLE_PYTHON|ON|build Python modules if enabled|
|ENABLE_JARS|ON|include jars with the install|
|ENABLE_JPEG|ON|build libjpeg driver and modules depending on it|
|ENABLE_J2K|ON|build openjpeg (jpeg2000) driver and modules depending on it|
|ENABLE_PCRE|ON|build PCRE (PERL Compatible Regular Expressions) library and modules dependent on it|
|ENABLE_UUID|ON|build uuid library and modules dependent on it|
|ENABLE_ZIP|ON|build zlib and modules dependent on it|
|PYTHON_VERSION||indicate which version of Python to prefer, e.g. "3" or "3.7"|
|BOOST_HOME||path to existing Boost installation (implies ENABLE_BOOST=ON)|
|PYTHON_HOME||path to existing Python installation (implies ENABLE_PYTHON=ON)|
|JPEG_HOME||path to existing libjpeg installation; if not provided, it will be built from source (implies ENABLE_JPEG=ON)|
|J2K_HOME||path to existing openjpeg installation; if not provided, it will be built from source (implies ENABLE_J2K=ON)|
|PCRE_HOME||path to existing pcre installation; if not provided, it will be built from source (implies ENABLE_PCRE=ON)|
|XML_HOME||path to existing Xerces installation; if not provided, it will be built from source|
|UUID_HOME||path to existing uuid library installation; if not provided, it will be built from source (Linux only)|
|ZIP_HOME||path to existing zlib installation; if not provided, it will be built from source|

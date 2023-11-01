# CMake build system for CODA-oss #

August 2018, Scott.Colcord@maxar.com <br />
April 2020, Keith.Wilhelm@maxar.com

CODA-oss now contains a CMake-based build system.

## Dependencies ##
### Required ###
* CMake >= 3.14, configured with "--qt-gui" if the qt gui is desired.
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


Building NITRO
---------------

CMake is the preferred build method. Version 3.14 or better is required.

Sample Build Scenario
---------------------

    mkdir build
    cd build
    cmake ..
    cmake --build . -j
    cmake --build . --target install
    ctest

Problems and Configurations
---------------------------
  - If your system compiler does not fully support C++11, you may have to
    specify a different one during the configure step. e.g.

        cmake -DCMAKE_C_COMPILER=/some/path/gcc/4.9.1/bin/gcc -DCMAKE_CXX_COMPILER=/...../bin/g++ ..

  - Pass -DCMAKE_INSTALL_PREFIX to set the install location.

  - Python and C++ bindings are built. Just make sure the relevant tools
    are on your PATH.  Java and MATLAB bindings are now in the **archive** directory
    and no longer built.

  - See the [coda-oss CMake build README](externals/coda-oss/cmake/README.md)
    for further build configuration information, particularly for Python-related
    details. The same options there may be passed to Nitro.

  - Build types `Release`, `RelWithDebInfo`, and `Debug` may be chosen
    - On Linux, debug symbols are available by default (`RelWithDebInfo`). Configure build type with [-DCMAKE_BUILD_TYPE](https://cmake.org/cmake/help/v3.0/variable/CMAKE_BUILD_TYPE.html)
    - On Windows, release type should be configured during the build and install steps

          cmake --build . --config Release -j
          cmake --build . --config Release --target install
      The CMake default build type `Debug` may not work with Python, unless the Python installation includes debug versions of the Python libraries.
  - If the CMake build system does not support a required feature that Waf does, create
    an issue or a pull request!


Building with Waf
-----------------
Waf is the legacy build system. Below are all of the options available.


    > python waf --help
    waf [command] [options]

    Main commands (example: ./waf build -j4)
      build    : builds the project
      clean    : removes the build files
      configure: configures the project
      dist     : makes a tarball for redistributing the sources
      distcheck: checks if the sources compile (tarball from 'dist')
      install  : installs the build files
      uninstall: removes the installed files

    Options:
      --version             show program's version number and exit
      -h, --help            show this help message and exit
      -j JOBS, --jobs=JOBS  amount of parallel jobs (8)
      -k, --keep            keep running happily on independent task groups
      -v, --verbose         verbosity level -v -vv or -vvv [default: 0]
      --nocache             ignore the WAFCACHE (if set)
      --zones=ZONES         debugging zones (task_gen, deps, tasks, etc)
      -p, --progress        -p: progress bar; -pp: ide output
      --targets=COMPILE_TARGETS
                            build given task generators, e.g. "target1,target2"
      --enable-warnings     Enable warnings
      --enable-debugging    Enable debugging
      --enable-64bit        Enable 64bit builds
      --enable-doxygen      Enable running doxygen
      --with-cflags=FLAGS   Set non-standard CFLAGS
      --with-cxxflags=FLAGS
                            Set non-standard CXXFLAGS (C++)
      --with-defs=DEFS      Use DEFS as macro definitions
      --with-optz=OPTZ      Specify the optimization level for optimized/release builds
      --libs-only           Only build the libs (skip building the tests, etc.)
      --shared              Build all libs as shared libs
      --disable-symlinks    Disable creating symlinks for libs
      --disable-java        Disable java (default)
      --with-java-home=JAVA_HOME
                            Specify the location of the java home
      --require-java        Require Java lib/headers (configure option)
      --nopyc               Do not install bytecode compiled .pyc files (configuration) [Default:install]
      --nopyo               Do not install optimised compiled .pyo files (configuration) [Default:install]
      --disable-python      Disable python
      --require-python      Require Python lib/headers (configure option)
      --enable-openjpeg     Enable openjpeg

      configuration options:
        -b BLDDIR, --blddir=BLDDIR
                            build dir for the project (configuration)
        -s SRCDIR, --srcdir=SRCDIR
                            src dir for the project (configuration)
        --prefix=PREFIX     installation prefix (configuration) [default: '/usr/local/']

      installation options:
        --destdir=DESTDIR   installation root [default: '']
        -f, --force         force file installation

      C Compiler Options:
        --check-c-compiler=CHECK_C_COMPILER
                            On this platform (linux) the following C-Compiler will be checked by default: "gcc icc suncc"

      C++ Compiler Options:
        --check-cxx-compiler=CHECK_CXX_COMPILER
                            On this platform (linux) the following C++ Compiler will be checked by default: "g++ icpc
                            sunc++"



Sample Build Scenario
---------------------
    > python waf configure --enable-debugging --prefix=installed
    > python waf build
    > python waf install


Enabling a debugger
-------------------
`-g` and its variants can be achieved at configure time using the
`--enable-debugging` switch at waf configure time

Memory Debugging
----------------
To ease debugging and memory leak detection, macros are used
to malloc, realloc and free information.
`NITF_MALLOC()`, `NITF_REALLOC()`, and `NITF_FREE()` should be used
instead of the stdlib.h functions.

If you defined `NITF_DEBUG` during compilation
(using configure, give the argument `--with-defs="-DNITF_DEBUG"`
and this will occur automatically), you will get
an memory image information dump every time you run an executable,
named `memory_trace.<pid>` where `<pid>` is the PID of the process
you just ran.  There is a verification tool located in
nitf/tests/verify, called `mem_sane.pl`.  If you run `mem_sane.pl`
with the memory trace as the single argument, you will get a
formatted output of all memory that is questionably allocated
or deallocated in the nitf library's calls.  Please, please, please
check your stuff.

C Conventions
-------------
In order to keep the C code easy to program and debug, and
above all, OO, we stick to certain conventions herein:

*	All constructors must be passed an error
	On failure, they return `NULL`, and populate the error

*	All destructors must `NULL` set the object after they
	are done deleting, and should check the object prior,
	to make sure that it has not been deleted already.
	This means, of course, that all destructors take a pointer
	to the object.  In practice, usually most of these, then,
	take double pointers (where usually you pass it a pointer
	by address)

*	All objects are in structs with an underscore in front of
	their name, and a typedef to the real name (.e.g.,
	`struct _nitf_PluginRegistry` => `nitf_PluginRegistry`)

*	All functions that are non-static should be wrapped in
	a `NITFAPI(return-val)` or `NITFPROT(return-val)` for protected
	data.

*	This allows for easy macro definitions in order to
	control the decoration algorithm for windows, and to assure
	that the import decoration and
	export decoration are identical (otherwise we cant use them)

*	IMPORTANT: The difference between `NITFAPI()` and `NITFPROT()`
	is that the C++ code binding generator exposes API() calls
	and ignores PROT() calls.

*	All enumerations and constants have a NITF/NITF20/NITF21
	prefix.  Along these lines, all functions and objects
	are prefixed with a 'namespace' (nitf/nitf20/nitf21).

Platforms
---------
While the ultimate goal is to be cross-platform and cross-language,
the C and C++ layers get the most support.

The Python layer gets some use for scripting convenience.

The MATLAB and JAVA layers have not been touched in years; they are
no longer built, code remains in the **archive** directory.

TREs need to be coded in C (only).

Before you commit
-----------------
*	Create a unit test for your all code you
	are adding

*	Compile and test. (`ctest`)

* A clang-format script is available at `externals/coda-oss/.clang-format`.
  Use it.

* Doxygen on root directory and view in
	browser the doxygen code (in nitf/doc/html/).

Doxygen Commenting
------------------
Please make an effort to write doxygen comments.  I know,
especially in C, that doxygen has some issues.  However,
its the best, cheapest thing we have, and its important
to have the APIs documented.  It will save me the trouble
of fixing it later, which will make me eternally grateful.


NITF Library Users: General Issues
----------------------------------
NITRO handles TREs by loading dynamic libraries at runtime. Therefore, you need
to make sure NITRO can find them.

* If you are building from source, the location will be compiled in, and
  you don't have to do anything extra.

* If you are working from a binary release, you will have to tell
  NITRO where the plugins are by setting the `NITF_PLUGIN_PATH`
  enviornment variable.
  This should look something like `<install>/share/nitf/plugins`.

* If you wish to use a custom TRE location, you can also specify that
  with `NITF_PLUGIN_PATH`.

Contact
---------

July 2020, Dan <dot> Smith <at> maxar.com

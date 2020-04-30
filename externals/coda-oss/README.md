     _________________________
    |   ____  _||_  ___  __   |
    |  /___ \/_||_\| __\/  \  |
    | //   \// || \||  \\ _ \ |
    | ||   [===||===]  ||(_)| |
    | ||   _|| || |||  ||__ | |
    | \\ _/ |\_||_/||__/|| || |
    |  \___/ \_||_/|___/|| || |
    |__________||_____________|

CODA is a set of modules, and each module, while complimentary to one another, has
a very specific and largely independent purpose.

Building CODA
--------------

CODA may be built using Waf or CMake. Below are all of the options available for Waf. See cmake/README.md for CMake build instructions.

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
      --enable-32bit        Enable 32bit builds
      --enable-doxygen      Enable running doxygen
      --with-cflags=FLAGS   Set non-standard CFLAGS
      --with-cxxflags=FLAGS
                            Set non-standard CXXFLAGS (C++)
      --with-linkflags=FLAGS
                            Set non-standard LINKFLAGS (C/C++)
      --with-defs=DEFS      Use DEFS as macro definitions
      --with-optz=OPTZ      Specify the optimization level for optimized/release builds
      --libs-only           Only build the libs (skip building the tests, etc.)
      --shared              Build all libs as shared libs
      --disable-symlinks    Disable creating symlinks for libs
      --unittests           Build-time option to run unit tests after the build has completed
      --disable-pcre        turn off PCRE
      --enable-pcre         turn on PCRE (default)
      --with-pcre-home=WITH_PCRE_HOME
                            Specify the PCRE Home - where PCRE is installed to
      --build-pcre          force building PCRE from scratch
      --nobuild-pcre        force building PCRE from scratch
      --disable-xml         turn off XML
      --enable-xml-layer=XML_LAYER
                            Specify the XML layer
      --with-xml-home=XML_HOME
                            Specify the XML Home - where the XML library is installed to
      --build-xml           force building XML library (expat) from scratch
      --nobuild-xml         force not building XML library from scratch
      --enable-sql-layer=SQL_LAYER
                            Specify the SQL layer
      --with-sql-home=SQL_HOME
                            Specify the SQL Home - where the SQL library is installed to
      --with-fft-home=FFT_HOME
                            Specify the FFT Home - where an FFT library is installed
      --with-fftw3-home=FFT3_HOME
                            Specify the FFT3 Home - where the FFT3 library is installed
      --disable-fft         turn off building FFT (default)
      --enable-fft          turn on FFT
      --enable-fft-double   turn on double precision FFT
      --build-fft           force building FFT library (fftw) from scratch
      --nobuild-fft         force building FFT library (fftw) from scratch
      --with-zip-home=ZIP_HOME
                            Specify the ZIP Home - where the ZIP library is installed
      --disable-zip         will not build the zip (zlib) library
      --enable-zip          will build the zip (libz) library if not found on the system (default)
      --build-zip           force building zip (zlib) library from scratch
      --nobuild-zip         force building zip (zlib) library from scratch
      --disable-uuid        will not build the uuid library
      --enable-uuid         will build the uuid library if not found on the system (default)
      --build-uuid          force building libuuid from scratch
      --nobuild-uuid        force building libuuid from scratch
      --with-uuid-home=UUID_HOME
                            Specify the UUID lib/headers home

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
                            On this platform (linux) the following C++ Compiler will be checked by default: "g++ icpc sunc++"


Sample Build Scenario
---------------------
    > cd modules
    > python waf configure --enable-debugging --prefix=installed
    > python waf build
    > python waf install


Enabling a debugger
-------------------
`-g` and its variants can be achieved at configure time using the
`--enable-debugging` switch at waf configure time

Common Errors
-------------
    Fatal Python error: initfsencoding: unable to load the file system codec
    ModuleNotFoundError: No module named 'encodings'

Problem: Python is unable to find its `modules` directory, necessary for using the Python C API.

Solution: Set the `PYTHONHOME` environment variable. On Windows, this may look like:

    set PYTHONHOME=C:\ProgramData\Anaconda3\

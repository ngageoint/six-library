Building NITRO
--------------

We primarily use Waf for building. Below are all of the options available.


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
While the ultimate goal is cross-platform, cross-language,
for this release, we are focusing only on C/C++.  For TREs,
they need to be coded in C (only).  

SVN Issues
----------
Please DONT POLLUTE SVN!  Don't put binaries in there,
unless you have a very good reason

*IMPORTANT: Before you commit:*

*	Please create a unit test for your all code you
	are adding 

*	Please compile and test.

*	Please 'make clean'

*   Please doxygen on root directory and view in
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
Dont forget to set your environment variable: `NITF_PLUGIN_PATH`.
For instance, in my location, it could be:

    setenv NITF_PLUGIN_PATH ~/nitf/plugins/i686-pc-linux-gnu/

NOTE: If you do not set the NITF_PLUGIN_PATH variable, the parser
will assume that the plugin path is "./plugins/" 
If you choose to ignore this variable, you MUST have your DSOs in
that directory (The Makefile will generate it in
nitf/plugins/@target@).


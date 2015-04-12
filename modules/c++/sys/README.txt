CONFIGURATION AND THREADING
===========================

If you simply type

./configure [--enable-threading]

The resultant library will be multi-threaded, using the
native thread package for your platform.  

If you do not wish to use the threaded implementation you
can turn it off:
./configure --disable-threading

Here is how to configure your system for threading

1.  To default (to system-native threads)
./configure


	    * defines:

	    * links to:
	      whatever your native package is

2.  To choose pthreads explicitly
./configure --with-thread-layer=pthreads

	    * defines:
	      __POSIX

	    * links to:
	      -lpthread

3.  To choose solaris threads explicitly.  This only works on Sun (SPARC)
./configure --with-thread-layer=solaris
	      
	    * defines:

	    * links to:
	      -lthread

4.  To choose netscape threads.  This works on any platform where
NSPR is installed, but if its not in the system directory, you
MUST also give it --with-thread-home=<your-nspr-home>
./configure --with-thread-layer=nspr (--with-thread-home=<your-nspr-home>)

	    * defines:
	      USE_NSPR_THREADS

	    * links to:
	      -lnspr4

5.  To choose IRIX sproc explicitly.
./configure --with-thread-layer=irix
	      
	    * defines:

	    * links to:

Other flags and defines:

_REENTRANT is defined for all threaded systems
-mt is defined under solaris

BUILDING UNDER WINDOWS (Without Configure)

To choose native windows threads.
When building your project, make sure that you are

     1) Defining _REENTRANT in your defines area
     2) Selecting either Multi-Thread Debug Library,
     or Multi-Thread Library (The default is Single-Thread
     which will lead to a disaster)

If you wish to use NSPR threads, you should define
USE_NSPR_THREADS along with _REENTRANT, and they will be selected
instead of the Windows default layer

TROUBLESHOOTING

I have tested sys 0.1.0 on the following platforms with the following
settings:

LINUX		SGI		SUN		WIN32
------------------------------------------------------------
pthreads	pthreads	pthreads	win32
nspr		sproc-based	solaris		nspr
unthreaded	unthreaded	unthreaded	unthreaded
------------------------------------------------------------

BUGS

All tests have passed successfully.  It is worth noting that a derived
package, casprThreads, fails under win32 native threads for the
queue tests.  It appears to be a bug inherited from the legacy CLoVErs 
implementation of the ConditionVarWin32.



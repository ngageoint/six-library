Building NITRO
----------------------
	For the time being, configuration is done with configure
	Type 
	$ ./configure --help 
	To see a list of options
	
	On most systems, the following suffices:
	$ ./configure 

	Now compile:

	$ make 
	
	If you want a clean make:
	
	$ make clean

	Will recursively delete many kinds of trash files:

	$ make fresh
	Simply cleans first and makes

NITF Library Programmers:
-------------------------
	Enabling a debugger:
        ~~~~~~~~~~~~~~~~~~~~
	'-g' and its variants can be achieved at configure time using the 
	'--enable-debugging'
	switch

	Memory Debugging:
        ~~~~~~~~~~~~~~~~~	
	To ease debugging and memory leak detection, macros are used
	to malloc, realloc and free information.
	NITF_MALLOC(), NITF_REALLOC(), and NITF_FREE() should be used
	instead of the stdlib.h functions.

	If you defined NITF_DEBUG during compilation
	(using configure, give the argument --with-defs="-DNITF_DEBUG"
	and this will occur automatically), you will get
	an memory image information dump every time you run an executable,
	named memory_trace.<pid> where <pid> is the PID of the process
	you just ran.  There is a verification tool located in
	nitf/tests/verify, called mem_sane.pl.  If you run mem_sane.pl
	with the memory trace as the single argument, you will get a
	formatted output of all memory that is questionably allocated
	or deallocated in the nitf library's calls.  Please, please, please
	check your stuff.

	TRE Debugging:
	~~~~~~~~~~~~~~
	If you define NITF_PRINT_TRES (--with-defs="-DNITF_PRINT_TRES"),
	the TRE hash will be dumped every time a handler is called.

	C Conventions:
	~~~~~~~~~~~~~~
	In order to keep the C code easy to program and debug, and
	above all, OO, we stick to certain conventions herein:

		All constructors must be passed an error
		On failure, they return NULL, and populate the error

		All destructors must NULL set the object after they
		are done deleting, and should check the object prior,
		to make sure that it has not been deleted already.
		This means, of course, that all destructors take a pointer
		to the object.  In practice, usually most of these, then,
		take double pointers (where usually you pass it a pointer
		by address)

		All objects are in structs with an underscore in front of
		their name, and a typedef to the real name (.e.g.,
		struct _nitf_PluginRegistry => nitf_PluginRegistry)

		All functions that are non-static should be wrapped in
		a NITFAPI(return-val) or NITFPROT(return-val) for protected
		data.  

		This allows for easy macro definitions in order to 
		control the decoration algorithm for windows, and to assure 
		that the import decoration and
		export decoration are identical (otherwise we cant use them)

		IMPORTANT: The difference between NITFAPI() and NITFPROT()
		is that the C++ code binding generator exposes API() calls
		and ignores PROT() calls.

		All enumerations and constants have a NITF/NITF20/NITF21
		prefix.  Along these lines, all functions and objects
		are prefixed with a 'namespace' (nitf/nitf20/nitf21).

	Platforms:
	~~~~~~~~~~
		While the ultimate goal is cross-platform, cross-language,
		for this release, we are focusing only on C/C++.  For TREs,
		they need to be coded in C (only).  

	SVN Issues:
	~~~~~~~~~~~
		Please DONT POLLUTE SVN!  Don't put binaries in there,
		unless you have a very good reason

		IMPORTANT: Before you commit:

		*	Please create a unit test for your all code you
			are adding 

		*	Please compile and test.

		*	Please 'make clean'
	
		*       Please doxygen on root directory and view in
			browser the doxygen code (in nitf/doc/html/).

	Doxygen Commenting:
	~~~~~~~~~~~~~~~~~~~
		Please make an effort to write doxygen comments.  I know,
		especially in C, that doxygen has some issues.  However,
		its the best, cheapest thing we have, and its important
		to have the APIs documented.  It will save me the trouble
		of fixing it later, which will make me eternally grateful.


NITF Library Users: General Issues:
-----------------------------------
	Dont forget to set your environment variable: NITF_PLUGIN_PATH.
	For instance, in my location, it could be:
	
	setenv NITF_PLUGIN_PATH ~/nitf/plugins/i686-pc-linux-gnu/
	
	NOTE: If you do not set the NITF_PLUGIN_PATH variable, the parser
	will assume that the plugin path is "./plugins/" 
	If you choose to ignore this variable, you MUST have your DSOs in
	that directory (The Makefile will generate it in
	nitf/plugins/@target@).


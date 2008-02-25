################################################
# We include this to get package specific info,
# more specifically, the LIBNAME and SOURCES
include ../build/Makefile

#############################################
# C O M P I L E R
# The compiler + pretool
CC =  gcc
# What are we defining on the compile line?
CCDEFINES = -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_REENTRANT -D__POSIX
# Dll Compile flags
DLLFLAGS =  -fPIC -shared
# Generic Compile Flags
CCFLAGS =  -fPIC -O1   -v  -Wall
# Where do the includes live?
INCLUDEPATH = $(EXTINCLUDEPATH) -I../include  
# The compile command
COMPILE = $(CC) $(CCFLAGS) $(CCDEFINES) $(INCLUDEPATH)

#############################################
# A R C H I V E R
ARCHIVE = ar -ru

#############################################
# L I N K E R
# Link flags
LFLAGS = 
# THIS library's path
DSODIR  = ../plugins/i686-pc-linux-gnu
# The testing directory
TESTDIR = i686-pc-linux-gnu
# The general lib path
LIBRARYPATH = -L$(DSODIR) $(EXTLIBPATH)  

# Libs to link with
LIBS =  $(EXTLIBS) -lpthread -ldl 

# The link suffixes
LINK = $(LIBRARYPATH) $(LFLAGS) $(LIBS)

#OBJS   = $(TESTS:.cpp=.o)
#EXE    = $(TESTS:.cpp=)

DSO = ${PLUGINS:.c=.so}


#################################################
#  This is the top level rule.  It makes your 
#  library
all: $(DSO)

#################################################
#  This target makes the "library directory" which 
#  is a platform-specific sub-directory named after 
#  the OS.  All the target does is create it if
#  it doesn't exist.  You dont need to touch this
#  rule, but you should make sure that $(PLUGINS)
#  depends on it

$(DSO): dsodir $(PLUGINS)

dsodir:
	\mkdir -p $(DSODIR)

doxy: 
	doxygen ../Doxyfile

clean: 
	perl -w /home/dpressel/SF.net/nitro-nitf/trunk/c/nitf/build//rfk.pl -f	

fresh:  clean
	make all

.SUFFIXES: .so .c
.c.so:
	$(COMPILE) $(DLLFLAGS) $< -o $(DSODIR)/$@ $(LINK)



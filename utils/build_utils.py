import sys, os, glob, re, fnmatch, zipfile, shutil
try:
    from cStringIO import StringIO
except:
    from StringIO import StringIO

#OS dependent file filters
WIN32_FILTER = r'.*(?:Win32).*'
UNIX_FILTER = r'.*(?:Unix|Solaris|Irix|Posix|NSPR).*'


def getPlatform():
    """ returns the platform name """
    platform = sys.platform
    if platform != 'win32':
        guess_locs = ['./build/config.guess', './config.guess', '../build/config.guess']
        for loc in guess_locs:
            if not os.path.exists(loc): continue
            try:
                out = os.popen('chmod +x %s' % loc)
                out.close()
                out = os.popen(loc, 'r')
                platform = out.readline()
                platform = platform.strip('\n')
                out.close()
            except:{}
            else:
                break
    return platform



def configure(target='lib', compiler=None, platform=None,
              debug=False, warnings=False,
              build64=False, threading=True, verbose=False,
              defines=None, includes=None, libpaths=None, libs=None):
    """
    This function essentially mimics the configure script, and sets up
    flags/libs based on the sytem in use. We use the config.guess script
    on all systems but Windows in order to guess the system.
    """
    
    if not platform:
        platform = getPlatform()
    
    _thread_defs="-D_REENTRANT"
    _thread_libs="pthread"
    _cxx_flags=""
    _64_flags=""
    _includes=""
    _cxx_defs=""
    
    if not compiler:
        #try to get the env var for CC
        compiler = os.environ.get('CC', 'gcc')
    
    ########################################
    # WIN32
    ########################################
    if platform.find('win32') >= 0:
        _debug_flags="/Zi"
        _warn_flags="/Wall"
        _verb_flags=""
        _64_flags=""
        _optz_med="-O2"
        _optz_fast="-O2"
        _optz_fastest="-O2"
        _optz_flags = _optz_med
        _thread_defs="-D_REENTRANT"
        _thread_libs=""
        
        _cxx_defs="/DWIN32 /UUNICODE /U_UNICODE"
        _cxx_flags="/EHs /GR"
        _cxx_optz_flags=_optz_flagsCXXFLAGS = ['/EHs', '/GR', warnings and '/Wall' or '/W3']
        _link_libs=""

        # choose the runtime to link against
        # [/MD /MDd /MT /MTd]
        if (compiler != 'gcc'):
            rtflag = '/M'
#            if opt_mex:
#                rtflag += 'D'
#            else:
            rtflag += 'T'
            # debug
            if debug:
                rtflag += 'd'
            _cxx_flags = '%s %s' % (_cxx_flags, rtflag)
    ########################################
    # LINUX
    ########################################
    elif platform.startswith('i686-pc'):
        _debug_flags="-g"
        _warn_flags="-Wall"
        _verb_flags="-v"
        _64_flags="-m64"
        _optz_med="-O1"
        _optz_fast="-O2"
        _optz_fastest="-O3"
        _optz_flags = _optz_med
        _thread_defs="-D_REENTRANT -D__POSIX"
        _thread_libs="pthread"
        
        _cxx_defs="-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE"
        _cxx_flags=""
        _cxx_optz_flags=_optz_flags
        _link_libs="dl nsl"
#        ar_flags="-ru"
#        dll_flags="-fPIC -shared"
        
    ########################################
    # APPLE 
    ########################################
    elif platform.find('apple') >= 0:
        _debug_flags="-g"
        _warn_flags="-Wall"
        _verb_flags="-v"
        _64_flags="-m64"
        _optz_med="-O1"
        _optz_fast="-O2"
        _optz_fastest="-O3"
        _optz_flags = _optz_med
        _thread_defs="-D_REENTRANT -D__POSIX"
        _thread_libs="pthread"

        _cxx_defs="-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE"
        _cxx_flags=""
        _cxx_optz_flags=_optz_flags
        _link_libs="dl"


    ########################################
    # SGI
    ########################################
    elif platform.startswith('mips-sgi-'):
#        dll_flags="-shared"
        _debug_flags="-g"
        _warn_flags="-fullwarn"
        _verb_flags="-v"
        _64_flags="-64"
        _optz_med="-O1"
        _optz_fast="-O2"
        _optz_fastest="-O3"
        _optz_flags=_optz_med
        _thread_defs="-D_REENTRANT"
        _thread_libs=""
        
        _cxx_defs="-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE"
        _cxx_flags="-LANG:std -LANG:ansi-for-init-scope=ON -ptused"
        _cxx_optz_flags=_optz_flags
        _link_libs="m"
#            AR="${CXX}"
#            ar_flags=" -ar -o"
    ########################################
    # SOLARIS
    ########################################
    elif platform.startswith('sparc-sun-'):
        _debug_flags="-g"
        _warn_flags=""
        _verb_flags="-v"
        _64_flags="-xtarget=generic64"
        _optz_med="-xO1"
        _optz_fast="-fast"
        _optz_fastest="-fast"
        _optz_flags=_optz_med
        _thread_defs="-mt"
        _thread_libs="thread"
        
        _cxx_defs="-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE"
        _cxx_flags=" -instances=static"
        _cxx_optz_flags=_optz_flags
        _link_libs="dl nsl socket"
#        AR="${CXX}"
#        ar_flags="-xar -o"
#        dll_flags="-G -Kpic"
    else:
        print 'Unsupported platform: %s' % platform
        sys.exit(1)

    #snag the defines, if any
    cxx_flags = defines or []
    cxx_includes = includes or []
    link_libpath = libpaths or []
    link_libs = libs or []
    
    local_lib = '%s/%s' % (target, platform)
    if build64:
        local_lib += '-64'
    # Due to name mangling, if the compiler is GCC, append the gnu
    # but, only append if it doesn't already end with gnu
    if compiler == 'g++':
        local_lib += '/gnu'
    

    #disable optz if we are debugging
    if debug:
        _cxx_optz_flags = ''
    else:
        _debug_flags = ''
    if not verbose:
        _verb_flags = ''
    if not warnings:
        _warn_flags = ''
    if not build64:
        _64_flags = ''
   
   
    cxx_includes = cxx_includes
    cxx_defs = _cxx_defs.split() + _thread_defs.split()
    cxx_flags = cxx_flags + _cxx_flags.split() + _optz_flags.split() + \
            _64_flags.split() + _verb_flags.split() + _debug_flags.split() + \
            _warn_flags.split()
    link_libs = link_libs + _link_libs.split() + _thread_libs.split()
    link_libpath = link_libpath

    #save these to the environment, as is, in case we want them later
    env = {}
    env['INCLUDES'] = cxx_includes
    env['DEFINES'] = cxx_defs
    env['FLAGS'] = cxx_flags
    env['LINK_LIBS'] = link_libs
    env['LINK_FLAGS'] = [] #TODO
    env['LINK_LIBPATH'] = link_libpath
    env['BUILD_DIR'] = local_lib
    env['PLATFORM'] = platform
    
    return env



class GlobDirectoryWalker:
    # a forward iterator that traverses a directory tree

    def __init__(self, directory, patterns=["*"]):
        self.stack = [directory]
        self.patterns = patterns
        self.files = []
        self.index = 0

    def __getitem__(self, index):
        while True:
            try:
                file = self.files[self.index]
                self.index = self.index + 1
            except IndexError:
                # pop next directory from stack
                self.directory = self.stack.pop()
                if os.path.isdir(self.directory):
                    self.files = os.listdir(self.directory)
                else:
                    self.files, self.directory = [self.directory], ''
                self.index = 0
            else:
                # got a filename
                fullname = os.path.join(self.directory, file)
                if os.path.isdir(fullname) and not os.path.islink(fullname):
                    self.stack.append(fullname)
                for p in self.patterns:
                    if fnmatch.fnmatch(file, p):
                        return fullname

def recursive_glob(directory, patterns=["*"]):
    return list(GlobDirectoryWalker(directory, patterns))


    
#borrowed from http://aspn.activestate.com/ASPN/Cookbook/Python/Recipe/465649
def extract( filename, dir ):
    zf = zipfile.ZipFile( filename )
    namelist = zf.namelist()
    dirlist = filter( lambda x: x.endswith( '/' ), namelist )
    filelist = filter( lambda x: not x.endswith( '/' ), namelist )
    # make base
    pushd = os.getcwd()
    if not os.path.isdir( dir ):
        os.mkdir( dir )
    os.chdir( dir )
    # create directory structure
    dirlist.sort()
    for dirs in dirlist:
        dirs = dirs.split( '/' )
        prefix = ''
        for dir in dirs:
            dirname = os.path.join( prefix, dir )
            if dir and not os.path.isdir( dirname ):
                os.mkdir( dirname )
            prefix = dirname
    # extract files
    for fn in filelist:
        try:
            out = open( fn, 'wb' )
            buffer = StringIO( zf.read( fn ))
            buflen = 2 ** 20
            datum = buffer.read( buflen )
            while datum:
                out.write( datum )
                datum = buffer.read( buflen )
            out.close()
        finally:
            print fn
    os.chdir( pushd )
import sys, os, glob, string, re, shutil, fnmatch, inspect

from build_utils import *

def addDefaultOptions(opts):
    """ Just returns some basic re-usable options """
    from SCons.Options import PathOption
    
    opts.Add('debug', 'Enable debugging', False)
    opts.Add('optz', 'Set optimizations', True)
    opts.Add('warnings', 'Enable warnings', False)
    opts.Add('prefix', 'where to install the files', None)
    opts.Add('defines', '-D compiler flags', None)
    opts.Add('includes', 'extra include paths, separated by ;', None)
    opts.Add('libpaths', 'extra lib paths, separated by ;', None)
    opts.Add('libs', 'extra libs', None)
    opts.Add('threading', 'Enable threading', True)
    opts.Add('verbose', 'Turn on compiler verbose', True)
    opts.Add('enable64', 'Make a 64-bit build', False)
    opts.Add('dynamic', 'Build a DLL', False)
    return opts


def getSourceFiles(dirname, ext='.c', platform=None):
    source = list(os.path.abspath(f) for f in  glob.glob('%s/*%s' % (dirname, ext)))
    #if a platform name is provided, filter the source files
    if platform:
        sourcefiles = []
        myRE = re.compile(WIN32_FILTER, re.I)
        if platform.find('win32') >= 0:
            myRE = re.compile(UNIX_FILTER, re.I)
        for x in source:
            if myRE.match(x) == None:
                sourcefiles.append(x)
        source = sourcefiles
    return source
    

def toBoolean(val):
    if val is not None:
        try:
            iVal = int(val)
            return iVal != 0
        except:{}
        return val.lower().strip() == 'true'
    return False


def doConfigure(env, dirname='lib'):
    opt_debug = toBoolean(env.subst('$debug'))
    opt_warnings = toBoolean(env.subst('$warnings'))
    opt_64bit = toBoolean(env.subst('$enable64'))
    opt_threading = toBoolean(env.subst('$threading'))
    opt_verbose = toBoolean(env.subst('$verbose'))
    opt_dynamic = toBoolean(env.subst('$dynamic'))
    
    defines = None
    if env.subst('$defines') and env.subst('$defines') != '0':
        defstr = env.subst('$defines')
        defines = defstr.split(" ")
    
    includes = []
    if env.subst('$includes') and env.subst('$include_paths') != '0':
        for s in env.subst('$includes').split(';'):
            includes.append(s)
    libpaths = []
    if env.subst('$libpaths') and env.subst('$lib_paths') != '0':
        for s in env.subst('$libpaths').split(';'):
            libpaths.append(s)
            # This is totally not right on windows and nobody uses scons
            # anywhere else that I know of
            #libpaths.append(s.startswith('-L') and s or '-L%s' % s)
    
    libs = []
    if env.subst('$libs') and env.subst('$libs') != '0':
        for s in env.subst('$libs').split(' '):
            libs.append(s)

            #libs.append(s.startswith('-l') and s or '-l%s' % s)
    
    #configure
    vars = configure(target=dirname, defines=defines, includes=includes,
                     libpaths=libpaths, libs=libs, debug=opt_debug,
                     warnings=opt_warnings, threading=opt_threading,
                     verbose=opt_verbose, build64=opt_64bit)
    
    #setup the standard environment variables
    env.Append(CPPPATH = vars['INCLUDES'])
    env.Append(CCFLAGS = vars['FLAGS'] + vars['DEFINES'])
    env.Append(LIBS = vars['LINK_LIBS'])
    env.Append(LIBPATH = vars['LINK_LIBPATH'])
    env.Append(DYNAMIC = opt_dynamic)
    env['PLATFORM'] = vars['PLATFORM']
    
    return vars['BUILD_DIR']


def makeLibs(env, build_libs, local_lib='lib', lib_path=None):
    lib_path = (lib_path or []) + [local_lib]
    libs = []
    for lib in build_libs:
        libname = lib['lib']
        if not lib.has_key('source'):
            source = getSourceFiles(lib.get('dir', 'src'), ext=lib.get('ext', '.c'))
        else:
            source = lib['source']
        depends = []
        if lib.has_key('depends'):
            depends += lib['depends']
        
        if lib.has_key('dynamic') and lib['dynamic']:
            libs.append(env.SharedLibrary(local_lib + '/' + libname, source, LIBS=depends + env['LIBS'], LIBPATH=lib_path + env['LIBPATH']))
        else:
            libs.append(env.StaticLibrary(local_lib + '/' + libname, source, LIBS=depends, LIBPATH=lib_path))
    return libs


#==============================================================================#

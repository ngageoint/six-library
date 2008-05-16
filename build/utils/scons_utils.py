import sys, os, glob, string, re, shutil, fnmatch, inspect

from build_utils import *

def addDefaultOptions(opts):
    """ Just returns some basic re-usable options """
    from SCons.Options import PathOption
    
    opts.Add('debug', 'Enable debugging', 0)
    opts.Add('optz', 'Set optimizations', 0)
    opts.Add('warnings', 'Enable warnings', 0)
    opts.Add('prefix', 'where to install the files', '0')
    opts.Add('defines', '-D compiler flags', 0)
    opts.Add('include_paths', 'extra include paths', 0)
    opts.Add('lib_paths', 'extra lib paths', 0)
    opts.Add('libs', 'extra libs', 0)
    opts.Add('threading', 'Enable threading', 1)
    opts.Add('verbose', 'Turn on compiler verbose', 1)
    opts.Add('enable64', 'Make a 64-bit build', 0)
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
    

def doConfigure(env, dirname='lib'):
    opt_warnings = env.subst('$warnings') and int(env.subst('$warnings'))
    opt_debug = env.subst('$debug') and int(env.subst('$debug'))
    opt_64bit = env.subst('$enable64') and int(env.subst('$enable64'))
    opt_threading = env.subst('$threading') and int(env.subst('$threading'))
    opt_verbose = env.subst('$verbose') and int(env.subst('$verbose'))
    
    defines = None
    if env.subst('$defines') and env.subst('$defines') != '0':
        defstr = env.subst('$defines')
        defines = defstr.split(" ")
    
    includes = None
    if env.subst('$include_paths') and env.subst('$include_paths') != '0':
        includeStr = env.subst('$include_paths')
        includes = includeStr.split(";")
    
    libpaths = None
    if env.subst('$lib_paths') and env.subst('$lib_paths') != '0':
        libStr = env.subst('$lib_paths')
        libpaths = libStr.split(";")
    
    libs = None
    if env.subst('$libs') and env.subst('$libs') != '0':
        libStr = env.subst('$libs')
        libs = libStr.split(" ")
    
    #configure
    vars = configure(target=dirname, defines=defines, includes=includes,
                     libpaths=libpaths, libs=libs, debug=opt_debug,
                     warnings=opt_warnings, threading=opt_threading,
                     verbose=opt_verbose, build64=opt_64bit)
    
    #setup the standard environment variables
    env.Append(CPPPATH = vars['INCLUDES'])
    env.Append(CCFLAGS = vars['FLAGS'] + ['-D%s' % d for d in vars['DEFINES']])
    env.Append(LIBS = vars['LINK_LIBS'])
    env.Append(LIBPATH = vars['LINK_LIBPATH'])
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

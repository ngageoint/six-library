import sys, os, types, re, fnmatch

import Options, Utils
from Configure import conf
from Build import BuildContext
from TaskGen import taskgen, feature, after, before


class CPPBuildContext(BuildContext):
    """
    Create a custom context for building C/C++ modules/plugins
    """
    def __init__(self):
        BuildContext.__init__(self)
        
    def load_envs(self):
        # override load_envs so we know when to set the default variants
        BuildContext.load_envs(self)
        variants = Options.options.variants
        if not variants:
            variants = self.env['DEFAULT_VARIANTS']
        elif len(variants) == 1 and variants[0].lower() == 'all':
            variants = self.all_envs.keys()
        #filter out default
        Options.options.variants = filter(lambda x: x != 'default', variants)
            
    def safeVersion(self, version):
        return re.sub(r'[^\w]', '.', version)
    
    def module(self, **modArgs):
        """
        Builds a module, along with optional tests.
        It makes assumptions, but most can be overridden by passing in args.
        """
        
        bld = self
        variant = Options.options.variants[0]
        env = bld.env_of_name(variant)
        env.set_variant(variant)
    
        modArgs = dict((k.lower(), v) for k, v in modArgs.iteritems())
        lang = modArgs.get('lang', 'c++')
        libExeType = {'c++':'cxx', 'c':'cc'}.get(lang, 'cxx')
        libName = '%s-%s' % (modArgs['name'], lang)
        path = modArgs.get('path',
                           'dir' in modArgs and bld.path.find_dir(modArgs['dir']) or bld.path)

        module_deps = map(lambda x: '%s-%s' % (x, lang), modArgs.get('module_deps', '').split())
        defines = modArgs.get('defines', '').split()
        uselib_local = module_deps + modArgs.get('uselib_local', '').split()
        uselib = modArgs.get('uselib', '').split() + ['CSTD', 'CRUN']
        includes = modArgs.get('includes', 'include').split()
        exportIncludes = modArgs.get('export_includes', 'include').split()
        libVersion = modArgs.get('version', None)
        
        if libVersion is not None and sys.platform != 'win32':
            targetName = '%s.%s' % (libName, self.safeVersion(libVersion))
        else:
            targetName = libName
        
        
        headersOnly = modArgs.get('headersonly', False)
        if not headersOnly:
            #build the lib
            lib = bld.new_task_gen(libExeType, env['LIB_TYPE'] or 'staticlib', includes=includes,
                    target=targetName, name=libName, export_incdirs=exportIncludes,
                    uselib_local=uselib_local, uselib=uselib, env=env.copy(),
                    defines=defines, path=path, install_path='${PREFIX}/lib')
            lib.find_sources_in_dirs(modArgs.get('source_dir', 'source'))
            lib.source = filter(modArgs.get('source_filter', None), lib.source)
            
            if libVersion is not None and sys.platform != 'win32' and Options.options.symlinks:
                symlinkLoc = '%s/%s' % (lib.install_path, env['staticlib_PATTERN'] % libName)
                bld.symlink_as(symlinkLoc, env['staticlib_PATTERN'] % lib.target, env=env)
            

        for f in path.find_dir('include').find_iter():
            relpath = f.relpath_gen(path)
            bld.install_files('${PREFIX}/%s' % os.path.dirname(relpath),
                              f.abspath())

        testNode = path.find_dir('tests')
        if testNode and not Options.options.libs_only:
            if not headersOnly:
                uselib_local = libName
            
            sourceExt = {'c++':'.cpp', 'c':'.c'}.get(lang, 'cxx')
            for test in filter(modArgs.get('test_filter', None),
                               testNode.find_iter(in_pat=['*%s' % sourceExt],
                                                  maxdepth=1, flat=True).split()):
                exe = bld.new_task_gen(libExeType, 'program', source=test,
                        uselib_local=uselib_local, uselib=uselib, env=env.copy(), includes='.',
                        target=os.path.splitext(test)[0], path=testNode,
                        install_path='${PREFIX}/share/%s/tests' % modArgs['name'])
        
        return env
    
    def plugin(self, **modArgs):
        """
        Builds a plugin (.so) and sets the install path based on the type of
        plugin (via the plugin kwarg).
        """
        bld = self
        variant = Options.options.variants[0]
        env = bld.env_of_name(variant)
        env.set_variant(variant)
        
        modArgs = dict((k.lower(), v) for k, v in modArgs.iteritems())
        libName = '%s-c++' % modArgs['name']
        plugin = modArgs.get('plugin', '')
        path = modArgs.get('path',
                           'dir' in modArgs and bld.path.find_dir(modArgs['dir']) or bld.path)

        module_deps = map(lambda x: '%s-c++' % x, modArgs.get('module_deps', '').split())
        defines = modArgs.get('defines', '').split() + ['PLUGIN_MODULE_EXPORTS']
        uselib_local = module_deps + modArgs.get('uselib_local', '').split()
        uselib = modArgs.get('uselib', '').split() + ['CSTD', 'CRUN']
        includes = modArgs.get('includes', 'include').split()
        exportIncludes = modArgs.get('export_includes', 'include').split()
        
        lib = bld.new_task_gen('cxx', 'shlib', includes=includes,
                target=libName, name=libName, export_incdirs=exportIncludes,
                uselib_local=uselib_local, uselib=uselib, env=env.copy(),
                defines=defines, path=path,
                install_path='${PREFIX}/share/%s/plugins' % plugin)
        lib.find_sources_in_dirs('source')
        lib.source = filter(modArgs.get('source_filter', None), lib.source)
    
    
    def program(self, **modArgs):
        """
        Builds a program (exe)defVariants
        """
        bld = self
        variant = Options.options.variants[0]
        env = bld.env_of_name(variant)
        env.set_variant(variant)
        
        modArgs = dict((k.lower(), v) for k, v in modArgs.iteritems())
        progName = modArgs['name']
        path = modArgs.get('path',
                           'dir' in modArgs and bld.path.find_dir(modArgs['dir']) or bld.path)

        deps = map(lambda x: '%s-c++' % x, modArgs.get('module_deps', '').split())
        defines = modArgs.get('defines', '').split()
        uselib_local = deps + modArgs.get('uselib_local', '').split()
        uselib = modArgs.get('uselib', '').split() + ['CSTD', 'CRUN']
        includes = modArgs.get('includes', 'include').split()
        source = modArgs.get('source', '').split() or None
        
        exe = bld.new_task_gen('cxx', 'program', source=source,
                               uselib_local=uselib_local, uselib=uselib,
                               env=env.copy(), target=progName, path=path,
                               install_path='${PREFIX}/bin')
        if not source:
            exe.find_sources_in_dirs(modArgs.get('sourcedir', 'source'))
            exe.source = filter(modArgs.get('source_filter', None), exe.source)
    
    
    def cloneVariants(self):
        bld = self
        
        # only cloning variants if NOT installing
        # if you are in an install phase, only the first variant will be
        # installed - this is in order to not have the files clash
        
        if not bld.is_install:
            variants = Options.options.variants and Options.options.variants[1:] or []
            for i, v in enumerate(variants):
                for j in range(len(bld.task_manager.groups)):
                    g = bld.task_manager.groups[j]
                    bld.add_group()
                    for obj in [] + g.tasks_gen:
                        obj.clone(v)


class GlobDirectoryWalker:
    """ recursively walk a directory, matching filenames """
    def __init__(self, directory, patterns=["*"]):
        self.stack = [directory]
        self.patterns = patterns
        self.files = []
        self.index = 0
        
    def __iter__(self):
        return self.next()
    
    def next(self):
        while True:
            try:
                file = self.files[self.index]
                self.index = self.index + 1
            except IndexError:
                # pop next directory from stack
                if len(self.stack) == 0:
                    raise StopIteration
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
                        yield fullname

def recursiveGlob(directory, patterns=["*"]):
    return GlobDirectoryWalker(directory, patterns)


def getPlatform(pwd=None, default=None):
    """ returns the platform name """
    platform = default or sys.platform
    
    if platform != 'win32':
        if not pwd:
            pwd = os.getcwd()
            
        locs = recursiveGlob(pwd, patterns=['config.guess'])
        for loc in locs:
            if not os.path.exists(loc): continue
            try:
                out = os.popen('chmod +x %s' % loc)
                out.close()
            except:{}
            try:
                out = os.popen(loc, 'r')
                platform = out.readline()
                platform = platform.strip('\n')
                out.close()
            except:{}
            else:
                break
    return platform



import zipfile
def unzipper(inFile, outDir):
    if not outDir.endswith(':') and not os.path.exists(outDir):
        os.mkdir(outDir)
    
    zf = zipfile.ZipFile(inFile)
    
    dirs = filter(lambda x: x.endswith('/'), zf.namelist())
    dirs.sort()
    
    for d in filter(lambda x: not os.path.exists(x),
                    map(lambda x: os.path.join(outDir, x), dirs)):
        os.mkdir(d)

    for i, name in enumerate(filter(lambda x: not x.endswith('/'), zf.namelist())):
        outFile = open(os.path.join(outDir, name), 'wb')
        outFile.write(zf.read(name))
        outFile.flush()
        outFile.close()


def set_options(opt):
    opt.tool_options('compiler_cc')
    opt.tool_options('compiler_cxx')
    opt.tool_options('misc')
    
    if Options.platform == 'win32':
        opt.tool_options('msvc')
    
    ############################################################################
    ### CONFIGURE OPTIONS
    opt.add_option('--enable-warnings', action='store_true', dest='warnings',
                   help='Enable warnings (configure option)')
    
    # the following three options are here for backwards compatibility
    # we recommend using the --default-variant option
    opt.add_option('--enable-debugging', action='store_true', dest='debugging',
                   help='Enable debugging')
    opt.add_option('--enable-64bit', action='store_true', dest='enable64', default=False,
                   help='Sets the default variant to be 64bit (deprecated - do not use)')
    opt.add_option('--enable-32bit', action='store_true', dest='enable32', default=False,
                   help='Sets the default variant to be 32bit')
    
    opt.add_option('--with-cflags', action='store', nargs=1, dest='cflags',
                   help='Set non-standard CFLAGS (configure option)', metavar='FLAGS')
    opt.add_option('--with-cxxflags', action='store', nargs=1, dest='cxxflags',
                   help='Set non-standard CXXFLAGS (C++) (configure option)', metavar='FLAGS')
    opt.add_option('--with-defs', action='store', nargs=1, dest='_defs',
                   help='Use DEFS as macro definitions (configure option)', metavar='DEFS')
    
    def splitVariants(option, opt_str, value, parser):
        setattr(parser.values, option.dest, value.split(','))
    
    opt.add_option('--default-variants', '--default-variant', action='callback',
                   type='str', dest='_defVariants',
                   help='Specify the default variant to build if none are specified at build time (configure option)',
                   callback=splitVariants, metavar='VARIANTS')
    opt.add_option('--with-optz', action='store',
                   choices=['med', 'fast', 'fastest'],
                   default='fastest', metavar='OPTZ',
                   help='Specify the optimization level for optimized/release builds (configure option)')
    
    ############################################################################
    ### BUILD OPTIONS
    opt.add_option('--enable-doxygen', action='store_true', dest='doxygen',
                   help='Enable running doxygen (build option)')
    opt.add_option('--libs-only', action='store_true', dest='libs_only',
                   help='Only build the libs (skip building the tests, etc.) (build option)')
    opt.add_option('--shared', action='store_true', dest='shared_libs',
                   help='Build all libs as shared libs (build option)')
    opt.add_option('--disable-symlinks', action='store_false', dest='symlinks',
                   default=True, help='Disable creating symlinks for libs (build option)')
    
    opt.add_option('--variants', '--variant', action='callback',
                   dest='variants', type='str',
                   metavar='VARIANTS', help='Specify the build variant(s) (build option) ' \
                        '[all, debug, debug64, release, release64]',
                   callback=splitVariants)
    

types_str = '''
#include <stdio.h>
int isBigEndian()
{
    long one = 1;
    return !(*((char *)(&one)));
}
int main()
{
    if (isBigEndian()) printf("bigendian=True\\n");
    else printf("bigendian=False\\n");
    printf("sizeof_int=%d\\n", sizeof(int));
    printf("sizeof_short=%d\\n", sizeof(short));
    printf("sizeof_long=%d\\n", sizeof(long));
    printf("sizeof_long_long=%d\\n", sizeof(long long));
    printf("sizeof_float=%d\\n", sizeof(float));
    printf("sizeof_double=%d\\n", sizeof(double));
    return 0;
}
'''

def detect(self):
    platform = getPlatform(default=Options.platform)
    
    self.check_message_custom('platform', '', platform, color='GREEN')
    self.check_tool('compiler_cc')
    self.check_tool('compiler_cxx')

    cxxCompiler = self.env["COMPILER_CXX"]
    ccCompiler = self.env["COMPILER_CC"]
    
    if ccCompiler == 'msvc':
        cxxCompiler = ccCompiler
        
    if not cxxCompiler or not ccCompiler:
        self.fatal('Unable to find C/C++ compiler')

    self.check_tool('misc')
    if Options.options.doxygen:
        self.find_program('doxygen', var='DOXYGEN')
    
    #Look for a ton of headers
    self.check_cc(header_name="inttypes.h")
    self.check_cc(header_name="unistd.h")
    self.check_cc(header_name="getopt.h")
    self.check_cc(header_name="malloc.h")
    self.check_cc(header_name="sys/time.h")
    self.check_cc(header_name="dlfcn.h")
    self.check_cc(header_name="fcntl.h")
    self.check_cc(header_name="check.h")
    self.check_cc(header_name="memory.h")
    self.check_cc(header_name="string.h")
    self.check_cc(header_name="strings.h")
    self.check_cc(header_name="stdbool.h")
    self.check_cc(header_name="stdlib.h")
    self.check_cc(header_name="stddef.h")
    self.check_cc(function_name='mmap', header_name="sys/mman.h")
    self.check_cc(function_name='memmove', header_name="string.h")
    self.check_cc(function_name='strerror', header_name="string.h")
    self.check_cc(function_name='bcopy', header_name="strings.h")
    self.check_cc(type_name='size_t', header_name='stddef.h')
    self.check_cc(fragment='int main(){const int i = 0; return 0;}',
                  define_name='HAVE_CONST', msg='Checking for const keyword')
    self.check_cc(fragment='int main(){unsigned short; return 0;}',
                  define_name='HAVE_UNSIGNED_SHORT', msg='Checking for unsigned short')
    self.check_cc(fragment='int main(){unsigned char i; return 0;}',
                  define_name='HAVE_UNSIGNED_CHAR', msg='Checking for unsigned char')
    self.check_cc(lib="m", mandatory=False, uselib_store='MATH')
    self.check_cc(lib="sqrt", mandatory=False, uselib_store='SQRT')
    
    self.check_cc(function_name='gettimeofday', header_name='sys/time.h')
    self.check_cc(function_name='BSDgettimeofday', header_name='sys/time.h')
    self.check_cc(function_name='gethrtime', header_name='sys/time.h')
    self.check_cc(function_name='getpagesize', header_name='unistd.h')
    self.check_cc(function_name='getopt', header_name='unistd.h')
    self.check_cc(function_name='getopt_long', header_name='getopt.h')
    
    self.check_cc(fragment='#include <math.h>\nint main(){if (!isnan(3.14159)) isnan(2.7183);}',
                  define_name='HAVE_ISNAN', msg='Checking for function isnan',
                  errmsg='not found')
    
    # Check for hrtime_t data type; some systems (AIX) seem to have
    # a function called gethrtime but no hrtime_t!
    frag = '''
    #ifdef HAVE_SYS_TIME_H
    #include <sys/time.h>
    int main(){hrtime_t foobar;}
    #endif
    '''
    self.check_cc(fragment=frag, define_name='HAVE_HRTIME_T',
                  msg='Checking for type hrtime_t', errmsg='not found')
    
    
    #find out the size of some types, etc.
    output = self.check(fragment=types_str, execute=1, msg='Checking system type sizes')
    t = Utils.to_hashtable(output or '')
    for k, v in t.iteritems():
        try:
            v = int(v)
        except:
            v = v.strip()
            if v == 'True':
                v = True
            elif v == 'False':
                v = False
        #v = eval(v)
        self.check_message_custom(k.replace('_', ' '), '', v)
        self.define(k.upper(), v)
    
    env = self.env
    env['PLATFORM'] = platform
    
    env['LIB_TYPE'] = Options.options.shared_libs and 'shlib' or 'staticlib'
    
    env.append_unique('CXXFLAGS', Options.options.cxxflags or '')
    env.append_unique('CCFLAGS', Options.options.cflags or '')
    if Options.options._defs:
        env.append_unique('CCDEFINES', Options.options._defs.split(','))
        env.append_unique('CXXDEFINES', Options.options._defs.split(','))

    appleRegex = r'i.86-apple-.*'
    linuxRegex = r'.*-.*-linux-.*|i686-pc-.*|linux'
    sparcRegex = r'sparc-sun.*'
    winRegex = r'win32'
    
    config = {'cxx':{}, 'cc':{}, 'both':{}}
    is64Bit = False

    #apple
    if re.match(appleRegex, platform):
        self.check_cc(lib="pthread", mandatory=True, uselib_store='THREAD')

        config['both']['debug']         = '-g'
        config['both']['warn']          = '-Wall'
        config['both']['verbose']       = '-v'
        config['both']['64']            = '-m64'
        config['both']['32']            = '-m32'
        config['both']['optz_med']      = '-O1'
        config['both']['optz_fast']     = '-O2'
        config['both']['optz_fastest']  = '-O3'
        
        config['both']['defines']       = '_FILE_OFFSET_BITS=64 _LARGEFILE_SOURCE __POSIX'.split()
        config['both']['linkflags']     = '-fPIC -dynamiclib'.split()
        config['both']['libs']          = {'DL':'dl', 'NSL':'nsl', 'THREAD':'pthread'}
        config['both']['flags']         = {'':'-fPIC -dynamiclib'.split(),
                                           'THREAD':'-D_REENTRANT'}

    #linux
    elif re.match(linuxRegex, platform):
        config['both']['debug']         = '-g'
        config['both']['warn']          = '-Wall'
        config['both']['verbose']       = '-v'
        config['both']['64']            = '-m64'
        config['both']['32']            = '-m32'
        config['both']['optz_med']      = '-O1'
        config['both']['optz_fast']     = '-O2'
        config['both']['optz_fastest']  = '-O3'
        
        config['both']['defines']       = '_FILE_OFFSET_BITS=64 _LARGEFILE_SOURCE __POSIX'.split()
        config['both']['linkflags']     = {'':'-Wl,-E -fPIC'.split()}
        config['both']['libs']          = {'DL':'dl', 'NSL':'nsl', 'THREAD':'pthread', 'MATH':'m'}
        #for some reason using CXXDEFINES_THREAD won't work w/uselib... so using FLAGS instead
        config['both']['flags']         = {'':'-fPIC', 'THREAD':'-D_REENTRANT'}
    
    #Solaris
    elif re.match(sparcRegex, platform):

        #check for the prpoer 64-bit flags
        if self.check_cxx(cxxflags='-xarch=sparc -m64 -errwarn'.split(), linkflags='-xarch=sparc -m64 -errwarn'.split(), mandatory=False, msg="Checking for -m64 compiler flag"):
            config['both']['64'] = '-xarch=sparc -m64'.split()
            config['both']['32'] = '-xarch=sparc -m32'.split()
            is64Bit = True
        elif self.check_cxx(cxxflags='-xtarget=generic64', linkflags='-xtarget=generic64', mandatory=False):
            config['both']['64'] = '-xtarget=generic64' 
            config['both']['32'] = '-xtarget=generic' 
            is64Bit = True
        
        config['both']['debug']         = '-g'
        config['both']['warn']          = ''
        config['both']['verbose']       = '-v'
        config['both']['optz_med']      = '-xO3'
        config['both']['optz_fast']     = '-xO4'
        config['both']['optz_fastest']  = '-fast'
        config['both']['linkflags_64'] = {'':config['both'].get('64', '')}
        config['both']['linkflags_32'] = {'':config['both'].get('32', '')}
        
        config['both']['flags']         = {'':'-KPIC'.split(), 'THREAD':'-mt'}
        config['cxx']['flags']          = {'':'-instances=global'.split()}
        
        config['both']['defines']       = '_FILE_OFFSET_BITS=64 _LARGEFILE_SOURCE'.split()
        config['both']['libs']          = {'DL':'dl', 'NSL':'nsl', 'SOCKET':'socket',
                                           'THREAD':'thread', 'MATH':'m',
                                           'CRUN':'Crun', 'CSTD':'Cstd'}
        
        #for all solaris envs
        env['shlib_CCFLAGS']            = ['-KPIC', '-DPIC']

    elif re.match(winRegex, platform):
        config['both']['debug']          = '/Zi /MTd'.split()
        config['both']['warn']           = '/Wall'
        config['both']['nowarn']         = '/W3 /wd4290'.split()
        config['both']['verbose']        = ''
        config['both']['64']             = ''
        config['both']['optz_med']       = '-O2 /MT'.split()
        config['both']['optz_fast']      = '-O2 /MT'.split()
        config['both']['optz_fastest']   = '-Ox /MT'.split()
        
        config['both']['libs']          = {'RPC':'rpcrt4', 'SOCKET':'Ws2_32'}
        config['both']['flags']         = {'':'/UUNICODE /U_UNICODE /EHs /GR'.split(), 'THREAD':'/D_REENTRANT'}
        config['both']['defines']       = '_FILE_OFFSET_BITS=64 _LARGEFILE_SOURCE WIN32'.split()
        
        # choose the runtime to link against
        # [/MD /MDd /MT /MTd]
        
    else:
        self.fatal('OS/platform currently unsupported: %s' % platform)
    
    def setEnv(env, config, key, suffix='FLAGS'):
        if key in config['cxx']:
            env.append_value('CXX%s' % suffix, config['cxx'][key])
        if key in config['cc']:
            env.append_value('CC%s' % suffix, config['cc'][key])
        if key in config['both']:
            env.append_value('CC%s' % suffix, config['both'][key])
            env.append_value('CXX%s' % suffix, config['both'][key])
    
    def checkLibs(env, config):
        libs = {}
        for k in ['cxx', 'cc', 'both']:
            if 'libs' in config[k]:
                libs.update(config[k]['libs'])
        for libStore, lib in libs.iteritems():
            self.check_cc(lib=lib, mandatory=True, uselib_store=libStore)
    
    def setFlags(env, config):
        flags = {'cxx':{}, 'cc':{}}
        if 'flags' in config['cxx']:
            flags['cxx'].update(config['cxx']['flags'])
        if 'flags' in config['cc']:
            flags['cc'].update(config['cc']['flags'])
        if 'flags' in config['both']:
            flags['cxx'].update(config['both']['flags'])
            flags['cc'].update(config['both']['flags'])
        
        for k, v in flags['cxx'].iteritems():
            suffix = k and '_%s' % k or ''
            env.append_value('CXXFLAGS%s' % suffix, v)
        for k, v in flags['cc'].iteritems():
            suffix = k and '_%s' % k or ''
            env.append_value('CCFLAGS%s' % suffix, v)
    
    def setLinkFlags(env, config, suffix=''):
        flags = {}
        for k in ['cxx', 'cc', 'both']:
            if 'linkflags%s' % suffix in config[k]:
                flags.update(config[k]['linkflags%s' % suffix])
        for k, v in flags.iteritems():
            s = k and '_%s' % k or ''
            env.append_value('LINKFLAGS%s' % s, v)
    
    # Check that the libs exist
    checkLibs(env, config)
    
    # General purpose flags
    setFlags(env, config)
    setLinkFlags(env, config)
    
    # DEFINES
    setEnv(env, config, 'defines', suffix='DEFINES')
    
    # WARNINGS
    if Options.options.warnings:
        setEnv(env, config, 'warn')
    else:
        setEnv(env, config, 'nowarn')
    
    # VERBOSE
    if Options.options.verbose:
        setEnv(env, config, 'verbose')
    
    #check if the system is 64-bit capable
    if not is64Bit:
        flags64 = config['both'].get('64', config['cxx'].get('64', None))
        linkFlags64 = config['both'].get('linkflags_64', config['cxx'].get('linkflags_64', {})).get('', None)
        if flags64:
            is64Bit = self.check_cxx(cxxflags=flags64, linkflags=linkFlags64, mandatory=False)
    
    defaultVariants = Options.options._defVariants
    if not defaultVariants:
        #see if they added some options
        default64 = is64Bit and not Options.options.enable32
        if Options.options.debugging:
            defaultVariants = ('debug%s' % (default64 and '64' or ''),)
        else:
            defaultVariants = ('release%s' % (default64 and '64' or ''),)
    
    # keep track of which variants we are supporting
    variants = []
    
    # DEBUG 32 VARIANT
    debug = env.copy()
    debug.set_variant('debug')
    self.set_env_name('debug', debug)
    setEnv(debug, config, 'debug')
    variants.append('debug')
    
    # RELEASE 32 VARIANT
    release = env.copy()
    optz = 'optz_%s' % Options.options.with_optz
    release.set_variant('release')
    self.set_env_name('release', release)
    setEnv(release, config, optz)
    variants.append('release')
    
    if is64Bit:
        # DEBUG 64 VARIANT
        debug64 = debug.copy()
        debug64.set_variant('debug64')
        self.set_env_name('debug64', debug64)
        variants.append('debug64')
        
        # RELEASE 64 VARIANT
        release64 = release.copy()
        release64.set_variant('release64')
        self.set_env_name('release64', release64)
        variants.append('release64')
        
        # set the debug/release 64 flags
        setEnv(debug64, config, '64')
        setEnv(release64, config, '64')
        setLinkFlags(debug64, config, '_64')
        setLinkFlags(release64, config, '_64')
    
    # now, set the debug/release 32 flags
    setEnv(debug, config, '32')
    setEnv(release, config, '32')
    setLinkFlags(debug, config, '_32')
    setLinkFlags(release, config, '_32')
    
    if len(defaultVariants) == 1 and defaultVariants[0].lower() == 'all':
        defaultVariants = tuple(variants)
    env['DEFAULT_VARIANTS'] = defaultVariants
    
    
@taskgen
@feature('untar')
def untar(tsk):
    import tarfile
    f = tsk.path.find_or_declare(tsk.fname)
    tf = tarfile.open(f.abspath(), 'r')
    p = tsk.path.abspath()
    for x in tf:
        tf.extract(x, p)
    tf.close()


@taskgen
@feature('unzip')
def unzip(tsk):
    f = tsk.path.find_or_declare(tsk.fname)
    unzipper(f.abspath(), tsk.path.abspath())


@taskgen
@feature('m4subst')
def m4subst(tsk):
    import re
    #similar to the subst in misc.py - but outputs to the src directory
    m4_re = re.compile('@(\w+)@', re.M)

    env = tsk.env
    infile = os.path.join(tsk.path.abspath(), tsk.input)
    outfile = os.path.join(tsk.path.abspath(), tsk.output)
    
    file = open(infile, 'r')
    code = file.read()
    file.close()

    # replace all % by %% to prevent errors by % signs in the input file while string formatting
    code = code.replace('%', '%%')

    s = m4_re.sub(r'%(\1)s', code)

    di = tsk.dict or {}
    if not di:
        names = m4_re.findall(code)
        for i in names:
            di[i] = env.get_flat(i) or env.get_flat(i.upper())
    
    file = open(outfile, 'w')
    file.write(s % di)
    file.close()
    if tsk.chmod: os.chmod(outfile, tsk.chmod)


@taskgen
@feature('commentUndefs')
def commentUndefs(tsk):
    import re
    env = tsk.env
    infile = os.path.join(tsk.path.abspath(), tsk.input)
    outfile = os.path.join(tsk.path.abspath(), tsk.output)
    
    file = open(infile, 'r')
    code = file.read()
    file.close()

    code = re.sub(r'(#undef[^\n]*)(\n)', r'/* \1 */\2', code)
    file = open(outfile, 'w')
    file.write(code)
    file.close()
    if tsk.chmod: os.chmod(outfile, tsk.chmod)


@taskgen
@feature('makeHeader')
def makeHeader(tsk):
    outfile = os.path.join(tsk.path.abspath(), tsk.output)
    dest = open(outfile, 'w')
    guard = '__CONFIG_H__'
    dest.write('#ifndef %s\n#define %s\n\n' % (guard, guard))

    for k in tsk.defs.keys():
        v = tsk.defs[k]
        if v is None:
            v = ''
        dest.write('\n#ifndef %s\n#define %s %s\n#endif\n' % (k, k, v))
    
    if hasattr(tsk, 'undefs'):
        for u in tsk.undefs:
            dest.write('\n#undef %s\n' % u)

    dest.write('\n#endif /* %s */\n' % guard)
    dest.close()
    if tsk.chmod: os.chmod(outfile, tsk.chmod)


@feature('cc', 'cxx')
@before('apply_obj_vars')
def fix_libs(self):
    """
    The apply_lib_vars task does not order the STATICLIBs based on dependencies,
    which is required for some OSes (Linux, Solaris)
    """
    env = self.env
    
    uselib = self.to_list(getattr(self, 'uselib', []))
    names = self.to_list(getattr(self, 'uselib_local', []))
    
    seen = {}
    tmp = names[:]
    while tmp:
        lib_name = tmp.pop(0)
        # visit dependencies only once
        if lib_name in seen:
            continue

        y = self.name_to_obj(lib_name)
        if not y: continue
        uselib_locals = y.to_list(getattr(y, 'uselib_local', []))
        
        if 'cshlib' in y.features or 'cprogram' in y.features:
            uselib_locals = filter(lambda x: 'cstaticlib' not in \
                                   self.name_to_obj(x).features, uselib_locals)
        
        seen[lib_name] = (y, uselib_locals)
        tmp.extend(uselib_locals)
        
    ordered = []
    allKeys = seen.keys()[:]
    while allKeys:
        for k in allKeys:
            found = False
            for ok in allKeys:
                if k == ok or found: continue
                found = k in seen[ok][1]
            if not found:
                ordered.append(k)
                allKeys.remove(k)
    
    env['STATICLIB'] = [seen[k][0].target for k in ordered]

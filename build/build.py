import sys, os, types, re, fnmatch

import Options, Utils
from Configure import conf
from TaskGen import taskgen, feature


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
    
    opt.add_option('--enable-warnings', action='store_true', dest='warnings',
                   help='Enable warnings')
    opt.add_option('--enable-debugging', action='store_true', dest='debugging',
                   help='Enable debugging')
    opt.add_option('--enable-64bit', action='store_true', dest='enable64',
                   help='Enable 64bit builds')
    opt.add_option('--with-cflags', action='store', nargs=1, dest='cflags',
                   help='Set non-standard CFLAGS')
    opt.add_option('--with-cxxflags', action='store', nargs=1, dest='cxxflags',
                   help='Set non-standard CXXFLAGS (C++)')
    opt.add_option('--with-optz', action='store', choices=['med', 'fast', 'fastest'],
                   default='med', help='Specify the optimization level for optimized/release builds')
    opt.add_option('--libs-only', action='store_true',
                   help='Only build the libs (skip building the tests, etc.)')
    

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


@conf
def configureCODA(self):
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
    self.find_program('doxygen', var='DOXYGEN', mandatory=False)
    
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
    
    env.append_unique('CXXFLAGS', Options.options.cxxflags or '')
    env.append_unique('CCFLAGS', Options.options.cflags or '')

    appleRegex = r'i.86-apple-.*'
    linuxRegex = r'.*-.*-linux-.*|i686-pc-.*|linux'
    sparcRegex = r'sparc-sun.*'
    winRegex = r'win32'
    
    config = {'cxx':{}, 'cc':{}}

    #apple
    if re.match(appleRegex, platform):
        env.append_value('LIB_DL', 'dl')
        env.append_value('LIB_NSL', 'nsl')
        env.append_value('LIB_THREAD', 'pthread')
        self.check_cc(lib='pthread', mandatory=True)

        config['cxx']['debug']          = '-g'
        config['cxx']['warn']           = '-Wall'
        config['cxx']['verbose']        = '-v'
        config['cxx']['64']             = '-m64'
        config['cxx']['optz_med']       = '-O1'
        config['cxx']['optz_fast']      = '-O2'
        config['cxx']['optz_fastest']   = '-O3'

        env.append_value('CXXDEFINES', '_FILE_OFFSET_BITS=64 _LARGEFILE_SOURCE __POSIX'.split())
        env.append_value('LINKFLAGS', '-fPIC -dynamiclib'.split())
        env.append_value('CXXFLAGS', '-fPIC')
        env.append_value('CXXFLAGS_THREAD', '-D_REENTRANT')

        config['cc']['debug']          = config['cxx']['debug']
        config['cc']['warn']           = config['cxx']['warn']
        config['cc']['verbose']        = config['cxx']['verbose']
        config['cc']['64']             = config['cxx']['64']
        config['cc']['optz_med']       = config['cxx']['optz_med']
        config['cc']['optz_fast']      = config['cxx']['optz_fast']
        config['cc']['optz_fastest']   = config['cxx']['optz_fastest']

        env.append_value('CCDEFINES', '_FILE_OFFSET_BITS=64 _LARGEFILE_SOURCE __POSIX'.split())
        env.append_value('CCFLAGS', '-fPIC -dynamiclib'.split())
        env.append_value('CCFLAGS_THREAD', '-D_REENTRANT')

    #linux
    elif re.match(linuxRegex, platform):
        env.append_value('LIB_DL', 'dl')
        env.append_value('LIB_NSL', 'nsl')
        env.append_value('LIB_THREAD', 'pthread')
        env.append_value('LIB_MATH', 'm')
        self.check_cc(lib='pthread', mandatory=True)

        if cxxCompiler == 'g++':
            config['cxx']['debug']          = '-g'
            config['cxx']['warn']           = '-Wall'
            config['cxx']['verbose']        = '-v'
            config['cxx']['64']             = '-m64'
            config['cxx']['optz_med']       = '-O1'
            config['cxx']['optz_fast']      = '-O2'
            config['cxx']['optz_fastest']   = '-O3'
            
            env.append_value('CXXDEFINES', '_FILE_OFFSET_BITS=64 _LARGEFILE_SOURCE __POSIX'.split())
            env.append_value('LINKFLAGS', '-Wl,-E -fPIC'.split())
            env.append_value('CXXFLAGS', '-fPIC')
            
            #for some reason using CXXDEFINES_THREAD won't work w/uselib... so using FLAGS instead
            env.append_value('CXXFLAGS_THREAD', '-D_REENTRANT')
        
        if ccCompiler == 'gcc':
            config['cc']['debug']          = '-g'
            config['cc']['warn']           = '-Wall'
            config['cc']['verbose']        = '-v'
            config['cc']['64']             = '-m64'
            config['cc']['optz_med']       = '-O1'
            config['cc']['optz_fast']      = '-O2'
            config['cc']['optz_fastest']   = '-O3'
            
            env.append_value('CCDEFINES', '_FILE_OFFSET_BITS=64 _LARGEFILE_SOURCE __POSIX'.split())
            env.append_value('CCFLAGS', '-fPIC'.split())
            
            #for some reason using CXXDEFINES_THREAD won't work w/uselib... so using FLAGS instead
            env.append_value('CCFLAGS_THREAD', '-D_REENTRANT')
    
    #Solaris
    elif re.match(sparcRegex, platform):
        env.append_value('LIB_DL', 'dl')
        env.append_value('LIB_NSL', 'nsl')
        env.append_value('LIB_SOCKET', 'socket')
        env.append_value('LIB_THREAD', 'thread')
        env.append_value('LIB_MATH', 'm')
        self.check_cc(lib='thread', mandatory=True)

        if cxxCompiler == 'sunc++':
            config['cxx']['debug']          = '-g'
            config['cxx']['warn']           = ''
            config['cxx']['verbose']        = '-v'
            config['cxx']['64']             = '-xtarget=generic64'
            config['cxx']['optz_med']       = '-xO2'
            config['cxx']['optz_fast']      = '-xO3'
            config['cxx']['optz_fastest']   = '-fast'

            env.append_value('CXXDEFINES', '_FILE_OFFSET_BITS=64 _LARGEFILE_SOURCE'.split())
            env.append_value('CXXFLAGS', '-KPIC'.split())
            env.append_value('CXXFLAGS_THREAD', '-mt')

        if ccCompiler == 'suncc':
            config['cc']['debug']          = '-g'
            config['cc']['warn']           = ''
            config['cc']['verbose']        = '-v'
            config['cc']['64']             = '-xtarget=generic64'
            config['cc']['optz_med']       = '-xO2'
            config['cc']['optz_fast']      = '-xO3'
            config['cc']['optz_fastest']   = '-fast'

            env.append_value('CCDEFINES', '_FILE_OFFSET_BITS=64 _LARGEFILE_SOURCE'.split())
            env.append_value('CCFLAGS', '-KPIC'.split())
            env.append_value('CCFLAGS_THREAD', '-mt')

        if Options.options.enable64:
            env.append_value('LINKFLAGS', config['cc']['64'].split())

    
    elif re.match(winRegex, platform):
        
        env.append_value('LIB_RPC', 'rpcrt4')
        env.append_value('LIB_SOCKET', 'Ws2_32')
        
        vars = {}
        vars['debug']          = '/Zi /MTd'.split()
        vars['warn']           = '/Wall'
        vars['nowarn']         = '/W3 /wd4290'.split()
        vars['verbose']        = ''
        vars['64']             = ''
        vars['optz_med']       = '-O2 /MT'.split()
        vars['optz_fast']      = '-O2 /MT'.split()
        vars['optz_fastest']   = '-Ox /MT'.split()
        
        # choose the runtime to link against
        # [/MD /MDd /MT /MTd]
        
        config['cxx'].update(vars)
        config['cc'].update(vars)

        defines = '_FILE_OFFSET_BITS=64 _LARGEFILE_SOURCE WIN32'.split()
        flags = '/UUNICODE /U_UNICODE /EHs /GR'.split()
        threadFlags = '/D_REENTRANT'
        
        env.append_value('CXXDEFINES', defines)
        env.append_value('CXXFLAGS', flags)
        env.append_value('CXXFLAGS_THREAD', threadFlags)
        
        env.append_value('CCDEFINES', defines)
        env.append_value('CCFLAGS', flags)
        env.append_value('CCFLAGS_THREAD', threadFlags)
    
    #CXX
    if Options.options.warnings:
        env.append_value('CXXFLAGS', config['cxx'].get('warn', ''))
        env.append_value('CCFLAGS', config['cc'].get('warn', ''))
    else:
        env.append_value('CXXFLAGS', config['cxx'].get('nowarn', ''))
        env.append_value('CCFLAGS', config['cc'].get('nowarn', ''))
    if Options.options.verbose:
        env.append_value('CXXFLAGS', config['cxx'].get('verbose', ''))
        env.append_value('CCFLAGS', config['cc'].get('verbose', ''))
    
    
    variant = env.copy() 
    if Options.options.debugging:
        variantName = '%s-debug' % platform
        variant.append_value('CXXFLAGS', config['cxx'].get('debug', ''))
        variant.append_value('CCFLAGS', config['cxx'].get('debug', ''))
    else:
        variantName = '%s-release' % platform
        optz = Options.options.with_optz
        variant.append_value('CXXFLAGS', config['cxx'].get('optz_%s' % optz, ''))
        variant.append_value('CCFLAGS', config['cc'].get('optz_%s' % optz, ''))
    
    if Options.options.enable64:
        variantName = '%s-64' % variantName
        variant.append_value('CXXFLAGS', config['cxx'].get('64', ''))
        variant.append_value('CCFLAGS', config['cc'].get('64', ''))
        
    self.set_env_name(variantName, variant)
    variant.set_variant(variantName)
    env.set_variant(variantName)
    self.setenv(variantName)
    
    env['VARIANT'] = variant['VARIANT'] = variantName


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

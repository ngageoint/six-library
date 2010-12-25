import Options, Build
from Configure import ConfigurationError
import os, subprocess, re
from os.path import join, dirname, abspath


def set_options(opt):
    opt.add_option('--disable-matlab', action='store_false', dest='matlab',
                   help='Disable matlab', default=True)
    opt.add_option('--with-matlab-home', action='store', dest='matlab_home',
                   help='Specify the location of the matlab home')
    opt.add_option('--require-matlab', action='store_true', dest='force_matlab',
               help='Require matlab (configure option)', default=False)

def detect(self):
    if not Options.options.matlab:
        return
    
    from build import recursiveGlob
    
    matlabHome = Options.options.matlab_home or self.env['MATLAB_HOME']
    matlabBin = matlabHome and join(matlabHome, 'bin')
    mandatory=Options.options.force_matlab
    
    if self.find_program('matlab', var='matlab', path_list=filter(None, [matlabBin]),
                      mandatory=mandatory):
        matlabBin = dirname(self.env['matlab'])
        if not matlabHome:
            matlabHome = join(matlabBin, os.pardir)
        
        platform = self.env['PLATFORM']
        
        #TODO put these in a utility somewhere
        appleRegex = r'i.86-apple-.*'
        linuxRegex = r'.*-.*-linux-.*|i686-pc-.*|linux'
        sparcRegex = r'sparc-sun.*'
        winRegex = r'win32'
        
        
        incDirs = map(lambda x: os.path.dirname(x),
                      recursiveGlob(abspath(join(matlabHome, 'extern')), ['mex.h']))
        
        exts = 'so dll lib'.split()
        libs = 'mx mex mat'.split()
        
        searches = []
        for x in exts:
            for l in libs:
                searches.append('*%s.%s' % (l, x))
        
        libDirs = map(lambda x: os.path.dirname(x),
                      recursiveGlob(matlabBin, searches))
        libDirs.extend(map(lambda x: os.path.dirname(x),
                      recursiveGlob(abspath(join(matlabHome, 'extern', 'lib')), searches)))
        
        if re.match(winRegex, platform):
            archdir = self.env['IS64BIT'] and 'win64' or 'win32'
            arch = self.env['IS64BIT'] and 'w64' or 'w32'
        else:
            #retrieve the matlab environment
            matlabEnvCmd = '%s -nojvm -nosplash -nodisplay -e' % self.env['matlab']
            out, err = subprocess.Popen(matlabEnvCmd.split(), stdout=subprocess.PIPE,
                                        stderr=subprocess.PIPE).communicate()
            matlabEnv = dict(map(lambda x: x.split('=', 1), filter(None, out.split('\n'))))
            #self.env['matlab_env'] = matlabEnv
            archdir = matlabEnv['ARCH']
            arch = archdir[-3:]
        self.env['MEX_EXT'] = '.mex%s' % arch
        
        filtered = filter(lambda x: archdir in x, libDirs)
        if filtered:
            libDirs = filtered
        libDirs = list(set(libDirs))
        
        self.env.append_value('CCFLAGS_MEX', '-DMATLAB_MEX_FILE'.split())
#        self.env.append_value('LINKFLAGS_MEX', '-Wl,-rpath-link,%s' % ':'.join(libDirs))
        try:
            env = self.env.copy()
            
            self.check(header_name='mex.h', define_name='HAVE_MEX_H',
                       includes=incDirs, uselib_store='MEX', uselib='MEX',
                       mandatory=True, env=env)
            
            libPrefix = ''
            if re.match(winRegex, platform):
                libPrefix = 'lib'
            
            # self.check(lib='%smat' % libPrefix, libpath=libDirs, uselib_store='MEX', uselib='MEX',
                       # type='cshlib', mandatory=True, env=env)
            self.check(lib='%smex' % libPrefix, libpath=libDirs, uselib_store='MEX', uselib='MEX',
                       type='cshlib', mandatory=True, env=env)
            self.check(lib='%smx' % libPrefix, libpath=libDirs, uselib_store='MEX', uselib='MEX',
                       type='cshlib', mandatory=True, env=env)
            
            if re.match(winRegex, platform):
                self.env.append_value('LINKFLAGS_MEX', '/EXPORT:mexFunction'.split())

        except ConfigurationError, ex:
            err = str(ex).strip()
            if err.startswith('error: '):
                err = err[7:]
            if mandatory:
                self.fatal(err)
            else:
                self.env['HAVE_MEX_H'] = None
                self.check_message_custom('matlab/mex', 'lib/headers', err, color='YELLOW')
        


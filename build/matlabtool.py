import Options, Build
from Configure import ConfigurationError
import os, subprocess
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
        
        
        #retrieve the matlab environment
        matlabEnvCmd = '%s -nojvm -nosplash -nodisplay -e' % self.env['matlab']
        out, err = subprocess.Popen(matlabEnvCmd.split(), stdout=subprocess.PIPE,
                                    stderr=subprocess.PIPE).communicate()
        matlabEnv = dict(map(lambda x: x.split('=', 1), filter(None, out.split('\n'))))
        self.env['matlab_env'] = matlabEnv
        
        incDirs = map(lambda x: os.path.dirname(x),
                      recursiveGlob(abspath(join(matlabHome, 'extern')), ['mex.h']))
        
        libDirs = map(lambda x: os.path.dirname(x),
                      recursiveGlob(matlabBin, ['*mx.so', '*mx.lib', '*mx.dll',
                                                '*mex.so', '*mex.lib', '*mex.dll']))
        
        if 'ARCH' in matlabEnv:
            arch = matlabEnv['ARCH']
            libDirs = filter(lambda x: x.endswith(arch), libDirs)
        libDirs = list(set(libDirs))
        
        self.env.append_value('CXXFLAGS_MEX', '-DMATLAB_MEX_FILE'.split())
#        self.env.append_value('LINKFLAGS_MEX', '-Wl,-rpath-link,%s' % ':'.join(libDirs))
        try:
            self.check(header_name='mex.h', define_name='HAVE_MEX_H',
                       includes=incDirs, uselib_store='MEX', uselib='MEX',
                       mandatory=True)
            self.check(lib='mat', libpath=libDirs, uselib_store='MEX', uselib='MEX',
                       type='cshlib', mandatory=True)
            self.check(lib='mx', libpath=libDirs, uselib_store='MEX', uselib='MEX',
                       type='cshlib', mandatory=True)
            self.check(lib='mex', libpath=libDirs, uselib_store='MEX', uselib='MEX',
                       type='cshlib', mandatory=True)
        except ConfigurationError, ex:
            err = str(ex).strip()
            if err.startswith('error: '):
                err = err[7:]
            if mandatory:
                self.fatal(err)
            else:
                self.env['HAVE_MEX_H'] = None
                self.check_message_custom('matlab/mex', 'lib/headers', err, color='YELLOW')
        

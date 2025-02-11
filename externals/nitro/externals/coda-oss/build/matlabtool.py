from waflib import Options, Build, Logs
from waflib.Errors import ConfigurationError
import os, subprocess, re, platform
from os.path import join, dirname, abspath

def expandIfSymlink(pathname):
    # os.path.realpath is supposed to do what we want, but it's buggy
    # readlink doesn't handle nested symlinks, hence the loop
    while os.path.islink(pathname):
        pathname = os.readlink(pathname)

    return pathname


def options(opt):
    opt.add_option('--disable-matlab', action='store_false', dest='matlab',
                   help='Disable matlab', default=True)
    opt.add_option('--with-matlab-home', action='store', dest='matlab_home',
                   help='Specify the location of the matlab home')
    opt.add_option('--require-matlab', action='store_true', dest='force_matlab',
               help='Require matlab (configure option)', default=False)

def configure(self):
    if not Options.options.matlab:
        return

    from build import recursiveGlob

    matlabHome = Options.options.matlab_home or self.env['MATLAB_HOME']
    matlabBin = matlabHome and join(matlabHome, 'bin')
    mandatory=Options.options.force_matlab

    # If you're on a 64-bit machine building 32-bit, you're not going to have
    # the right Mex files sitting around
    skipMatlab = '64' in platform.machine() and not self.env['IS64BIT']
    if skipMatlab and mandatory:
        self.fatal('32-bit Matlab not available on 64-bit machines')

    if not skipMatlab and self.find_program('matlab', var='matlab', path_list=[_f for _f in [matlabBin] if _f],
                                            mandatory=mandatory):

        matlabPath = expandIfSymlink(self.env['matlab'])
        matlabBin = dirname(matlabPath)
        if not matlabHome:
            matlabHome = join(matlabBin, os.pardir)

        #TODO put these in a utility somewhere
        winRegex = r'win32'

        incDirs = [os.path.dirname(x) for x in recursiveGlob(abspath(join(matlabHome, 'extern')), ['mex.h'])]

        exts = 'so dll lib'.split()
        libs = 'mx mex mat'.split()

        searches = []
        for x in exts:
            for l in libs:
                searches.append('*%s.%s' % (l, x))

        libDirs = [os.path.dirname(x) for x in recursiveGlob(matlabBin, searches)]
        libDirs.extend([os.path.dirname(x) for x in recursiveGlob(abspath(join(matlabHome, 'extern', 'lib')), searches)])

        mexExtCmd = os.path.join(matlabBin, 'mexext')
        if re.match(winRegex, self.env['PLATFORM']):
            archdir = self.env['IS64BIT'] and 'win64' or 'win32'
            mexExtCmd += '.bat'
        else:
            #retrieve the matlab environment
            matlabEnvCmd = '%s -nojvm -nosplash -nodisplay -e' % self.env['matlab']
            out, err = subprocess.Popen(matlabEnvCmd.split(), stdout=subprocess.PIPE,
                                        stderr=subprocess.PIPE,
                                        universal_newlines=True).communicate()

            for line in out.split('\n'):
                keyVal = line.split('=', 1)
                if len(keyVal) == 2 and keyVal[0] == 'ARCH':
                    archdir = keyVal[1]
                    break

        if not os.path.exists(mexExtCmd):
            Logs.warn('Unable to find {0}. Disabling MATLAB'.format(mexExtCmd))
            return False

        # Get the appropriate mex extension.  Matlab provides a script to
        # tell us this.
        out, err = subprocess.Popen(mexExtCmd, stdout=subprocess.PIPE,
                                        stderr=subprocess.PIPE,
                                        universal_newlines=True).communicate()

        self.env['MEX_EXT'] = '.' + out.rstrip()

        filtered = [x for x in libDirs if archdir in x]
        if filtered:
            libDirs = filtered
        libDirs = list(set(libDirs))

        self.env.append_value('CFLAGS_MEX', '-DMATLAB_MEX_FILE'.split())
#        self.env.append_value('LINKFLAGS_MEX', '-Wl,-rpath-link,%s' % ':'.join(libDirs))

        try:
            env = self.env.derive()

            if self.check(header_name='mex.h', define_name='HAVE_MEX_H',
                    includes=incDirs, uselib_store='MEX', uselib='MEX',
                    mandatory=True, env=env):
                        self.env['HAVE_MATLAB'] = True
                        self.undefine('HAVE_MEX_H')

            libPrefix = ''
            if re.match(winRegex, self.env['PLATFORM']):
                libPrefix = 'lib'

            # self.check(lib='%smat' % libPrefix, libpath=libDirs, uselib_store='MEX', uselib='MEX',
                       # type='cshlib', mandatory=True, env=env)
            self.check(lib='%smex' % libPrefix, libpath=libDirs, uselib_store='MEX', uselib='MEX',
                       mandatory=True, env=env)
            self.check(lib='%smx' % libPrefix, libpath=libDirs, uselib_store='MEX', uselib='MEX',
                       mandatory=True, env=env)

            if re.match(winRegex, self.env['PLATFORM']):
                self.env.append_value('LINKFLAGS_MEX', '/EXPORT:mexFunction'.split())

        except ConfigurationError as ex:
            err = str(ex).strip()
            if err.startswith('error: '):
                err = err[7:]
            if mandatory:
                self.fatal(err)
            else:
                self.undefine('HAVE_MEX_H')
                self.msg('matlab/mex lib/headers', err, color='YELLOW')

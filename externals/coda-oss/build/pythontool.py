from waflib import Options
import os, re

def options(opt):
    opt.load('python')
    opt.add_option('--disable-python', action='store_false', dest='python',
                   help='Disable python', default=True)
    opt.add_option('--python-version', action='store', dest='python_version',
                   default=None, help='Specify the minimum python version')
    opt.add_option('--require-python', action='store_true', dest='force_python',
                   help='Require Python program (configure option)', default=False)
    opt.add_option('--python-dev', action='store_true', dest='python_dev',
                   help='Require Python lib/headers (configure option)', default=False)
    
def configure(conf):
    if Options.options.python:
    
        try:
            conf.load('python')
            if not conf.env['PYTHON']:
                raise Exception('python not found')
        except Exception as e:
            if Options.options.force_python:
                raise e
            else:
                return
        
        pyver = Options.options.python_version
        if pyver:
            pyver = map(int, pyver.split('.'))
        conf.check_python_version(minver=pyver)
        
        # The waf python tool uses distutils.msvccompiler, tell it that we've already setup msvc.
        winRegex = r'win32'
        if re.match(winRegex, conf.env['PLATFORM']):
            os.environ['DISTUTILS_USE_SDK'] = '1'
            os.environ['MSSdk'] = '1'

        try:
            conf.check_python_headers()
            conf.msg('Can build python bindings', 'yes', color='GREEN')
        except Exception as ex:
            print ex
            err = str(ex).strip()
            if err.startswith('error: '):
                err = err[7:]
            err = err + ' (Is python built with -fPIC?)'
            if Options.options.python_dev:
                conf.fatal(err)
            else:
                conf.msg('Python lib/headers', err, color='YELLOW')
        
        # The python config tool in waf 1.6.11 tries to override our msvc config..
        if re.match(winRegex, conf.env['PLATFORM']):
            conf.env['CFLAGS_PYEXT'] = []
            conf.env['CXXFLAGS_PYEXT'] = []
            conf.env['LINKFLAGS_PYEXT'] = []
            

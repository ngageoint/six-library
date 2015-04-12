import Options

def set_options(opt):
    opt.tool_options('python')
    opt.add_option('--disable-python', action='store_false', dest='python',
                   help='Disable python', default=True)
    opt.add_option('--python-version', action='store', dest='python_version',
                   default=None, help='Specify the minimum python version')
    opt.add_option('--require-python', action='store_true', dest='force_python',
                   help='Require Python program (configure option)', default=False)
    opt.add_option('--python-dev', action='store_true', dest='python_dev',
                   help='Require Python lib/headers (configure option)', default=False)
    
def detect(conf):
    if Options.options.python:
    
        try:
            conf.check_tool('python')
            if not conf.env['PYTHON']:
                raise Exception('python not found')
        except Exception, e:
            if Options.options.force_python:
                raise e
            else:
                return
        
        pyver = Options.options.python_version
        if pyver:
            pyver = map(int, pyver.split('.'))
        conf.check_python_version(minver=pyver)
        
        try:
            conf.check_python_headers()
        except Exception, ex:
            err = str(ex).strip()
            if err.startswith('error: '):
                err = err[7:]
            if Options.options.python_dev:
                conf.fatal(err)
            else:
                conf.check_message_custom('Python', 'lib/headers', err, color='YELLOW')

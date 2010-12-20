import Options

def set_options(opt):
    opt.tool_options('python')
    opt.add_option('--disable-python', action='store_false', dest='python',
                   help='Disable python', default=True)
    opt.add_option('--python-version', action='store', dest='python_version',
                   default=None, help='Specify the minimum python version')
    opt.add_option('--python-dev', action='store_true', dest='python_dev',
                   help='Require Python lib/headers (configure option)', default=False)
    
def detect(conf):
    if Options.options.python:
        conf.check_tool('python')
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

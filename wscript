import os
import Scripting, Options
from Build import BuildContext

VERSION = '2.5-dev'
APPNAME = 'nitro'
srcdir  = '.'
blddir  = 'target'


class LogContext(BuildContext):
    def load_envs(self):
        BuildContext.load_envs(self)
        
        if Options.options.with_log:
            #create a log
            try:
                log = open(Options.options.with_log, 'a')
            except:{}
            else:
                self.log = log
    
    def install(self):
        BuildContext.install(self)
        if hasattr(self, 'log') and self.log:
            #close the log, since this marks the end of the build
            self.log.close()
build_context = LogContext

TOOLS_DIR = './build/utils/waf'

def set_options(opt):
    opt.tool_options('coda', tooldir=TOOLS_DIR)
    opt.sub_options('c c++ java')
    opt.add_option('--with-log', action='store', metavar="FILE",
                   help='Specify a filename for log output')
    

def configure(conf):
    conf.env['APPNAME'] = APPNAME
    conf.env['VERSION'] = VERSION
    conf.check_tool('coda', tooldir=TOOLS_DIR)
    conf.configureCODA()
    conf.sub_config('c c++ java')

def build(bld):
    bld.add_subdirs('c c++ java')
    
    #since waf 1.5.5 we need to do this...
    from coda import removeVariantTasks
    removeVariantTasks(bld, 'default')

def distclean(context):
    context.recurse('c c++ java')
    Scripting.distclean(context)


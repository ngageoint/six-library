import os
import Scripting, Options
from Build import BuildContext
from waflib.Tools import waf_unit_test

VERSION = '2.7-dev'
APPNAME = 'nitro'
top     = '.'
out     = 'target'

TOOLS = 'build swig javatool pythontool matlabtool'
DIRS = 'c c++ java python mex'
#external

def options(opt):
    opt.tool_options(TOOLS, tooldir='build')
    opt.sub_options(DIRS)

def configure(conf):
    conf.env['APPNAME'] = APPNAME
    conf.env['VERSION'] = VERSION
    conf.check_tool(TOOLS, tooldir='build')
    
    conf.sub_config(DIRS)

def build(bld):
    bld.add_subdirs(DIRS)
    bld.add_post_fun(waf_unit_test.summary)

def distclean(context):
    context.recurse(DIRS)
    Scripting.distclean(context)

#def init(context):
#    """
#    Override the lockfile - which allows you to build on multiple platforms
#    from the same sandbox without needing to re-configure every time you swap
#    between platforms.
#    
#    In other words, configure once per platform, then you can build for each
#    """
#    from build import getPlatform
#    platform = getPlatform()
#    Options.lockfile = '.lock-%s' % platform

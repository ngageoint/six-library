import os
import Scripting, Options
from Build import BuildContext

VERSION = '2.7-dev'
APPNAME = 'nitro'
srcdir  = '.'
blddir  = 'target'

TOOLS = 'build swig javatool pythontool matlabtool'
DIRS = 'c c++ java python mex'
#external

def set_options(opt):
    opt.tool_options(TOOLS, tooldir='build')
    opt.sub_options(DIRS)

def configure(conf):
    conf.env['APPNAME'] = APPNAME
    conf.env['VERSION'] = VERSION
    conf.check_tool(TOOLS, tooldir='build')
    
    conf.sub_config(DIRS)

def build(bld):
    bld.add_subdirs(DIRS)

def distclean(context):
    context.recurse(DIRS)
    Scripting.distclean(context)

def init(context):
    """
    Override the lockfile - which allows you to build on multiple platforms
    from the same sandbox without needing to re-configure every time you swap
    between platforms.
    
    In other words, configure once per platform, then you can build for each
    """
    from build import getPlatform
    platform = getPlatform()
    Options.lockfile = '.lock-%s' % platform

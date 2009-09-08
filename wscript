import os
import Scripting, Options
from Build import BuildContext

VERSION = '2.5-dev'
APPNAME = 'nitro'
srcdir  = '.'
blddir  = 'target'


def set_options(opt):
    opt.tool_options('build', tooldir='build')
    opt.sub_options('c c++ java')

def configure(conf):
    conf.env['APPNAME'] = APPNAME
    conf.env['VERSION'] = VERSION
    conf.check_tool('build', tooldir='build')
    conf.configureCODA()
    conf.sub_config('c c++ java external')

def build(bld):
    bld.add_subdirs('c c++ java external')

def distclean(context):
    context.recurse('c c++ java external')
    Scripting.distclean(context)


import os
from os.path import join
from waflib import Scripting, Options, Context
from build import CPPOptionsContext

VERSION = '1.0'
APPNAME = 'SIX'
Context.APPNAME = APPNAME
top  = '.'
out  = 'target'

TOOLS = 'build swig matlabtool pythontool'
TOOLS_DIR = join('externals', 'coda-oss', 'build')
DIRS = 'externals six'

def options(opt):
    opt.load(TOOLS, tooldir=TOOLS_DIR)
    opt.recurse(DIRS)

def configure(conf):
    conf.load(TOOLS, tooldir=TOOLS_DIR)
    conf.recurse(DIRS)
    
def build(bld):
    bld.launch_dir = join(bld.launch_dir, 'six')
    bld.recurse(DIRS)

def distclean(context):
    context.recurse('modules projects')
    Scripting.distclean(context)

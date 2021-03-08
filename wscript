import os
import subprocess
from os.path import join
from waflib import Scripting, Options, Context
from build import CPPOptionsContext

# Version is set in six/modules directory
# Set it there because someone may be building SIX as part of another repo and
# skipping our top-level wscript
APPNAME = 'SIX'
Context.APPNAME = APPNAME
top  = '.'
out  = 'target'

TOOLS = 'build swig matlabtool pythontool'
coda_oss = 'coda-oss'
TOOLS_DIR = join('externals', coda_oss, 'build')
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
    context.recurse(DIRS)
    Scripting.distclean(context)


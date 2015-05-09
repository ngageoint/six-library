import os
from os.path import join
from waflib import Scripting, Options
from build import CPPOptionsContext

VERSION = '1.0'
APPNAME = 'SIX'
top  = '.'
out  = 'target'

TOOLS = 'build swig matlabtool pythontool'
TOOLS_DIR = join('externals', 'coda-oss', 'build')
DIRS = 'externals modules projects'

def options(opt):
    opt.load(TOOLS, tooldir=TOOLS_DIR)
    opt.recurse(DIRS)

def configure(conf):
    conf.load(TOOLS, tooldir=TOOLS_DIR)
    conf.recurse(DIRS)
    
def build(bld):
    bld.recurse(DIRS)

    if not bld.targets:
        bld.targets = 'cphd-c++,six.sicd-c++,six.sidd-c++,six-samples'
        if 'HAVE_CSM' in bld.env:
            bld.targets += ',six-csm,vts'
        if bld.is_defined('HAVE_MEX_H'):
            bld.targets += ',nitf_image,nitf_metadata,xml_metadata'
        if 'PYTHON' in bld.env and bld.env['PYTHON'] and bld.is_defined('HAVE_PYTHON_H'):
            bld.targets += ',except-python,io-python,logging-python,math.linear-python,math.poly-python,mem-python,scene-python,sio.lite-python,sys-python,types-python'

def distclean(context):
    context.recurse('modules projects')
    Scripting.distclean(context)

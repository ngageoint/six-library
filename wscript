import os
from os.path import join
from waflib import Scripting, Options

VERSION = '1.0'
APPNAME = 'SIX'
top  = '.'
out  = 'target'

def options(opt):
    opt.load('build', tooldir=join(opt.path.abspath(), 'modules/build'))
    opt.recurse('modules projects')

def configure(conf):
    conf.load('build', tooldir='./modules/build/')
    conf.recurse('modules projects')
    
def build(bld):
    bld.recurse('modules projects')

    if not bld.targets:
        bld.targets = 'six.sicd-c++,six.sidd-c++,six-samples'
        if 'HAVE_CSM' in bld.env:
            bld.targets += ',six-csm,vts'
        if bld.is_defined('HAVE_MEX_H'):
            bld.targets += ',nitf_image,nitf_metadata,xml_metadata'
        if 'PYTHON' in bld.env and bld.env['PYTHON'] and bld.is_defined('HAVE_PYTHON_H'):
            bld.targets += ',six-python'

def distclean(context):
    context.recurse('modules projects')
    Scripting.distclean(context)

import os
from os.path import join
from waflib import Scripting, Options

VERSION = '1.0'
APPNAME = 'SIDD'
top  = '.'
out  = 'target'

def options(opt):
    opt.load('build', tooldir=join(opt.path.abspath(), 'modules/build'))
    opt.recurse('modules projects')

def configure(conf):
    conf.load('build', tooldir='./modules/build/')
    conf.recurse('modules projects')
    
    #add some extra config for the samples, so they can build w/sio.lite
    conf.env.append_unique('CXXDEFINES_SIX_SAMPLES', 'USE_SIO_LITE')
    sioLib = conf.env['PLATFORM'].startswith('win') and 'libsio.lite-c++' or 'sio.lite-c++.1.0'
    conf.env.append_unique('LIB_SIX_SAMPLES', sioLib)
    conf.env.append_value('CPPPATH_SIX_SAMPLES', os.path.join(conf.path.abspath(), 'sio.lite', 'include'))
    conf.env.append_value('LIBPATH_SIX_SAMPLES', os.path.join(conf.path.abspath(), 'sio.lite'))
    
def build(bld):
    #Options.options.libs_only = True
    bld.recurse('modules projects')

def distclean(context):
    context.recurse('modules projects')
    Scripting.distclean(context)

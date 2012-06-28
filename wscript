import os
from os.path import join
import Scripting, Options
from Build import BuildContext

VERSION = '1.0'
APPNAME = 'SIDD'
top  = '.'
out  = 'target'

def options(opt):
    opt.tool_options('build', tooldir=join(opt.path.abspath(), 'modules/build'))
    opt.sub_options('modules projects')

def configure(conf):
    conf.check_tool('build', tooldir='./modules/build/')
    conf.sub_config('modules projects')
    
    #add some extra config for the samples, so they can build w/sio.lite
    conf.env.append_unique('CXXDEFINES_SIX_SAMPLES', 'USE_SIO_LITE')
    sioLib = conf.env['PLATFORM'].startswith('win') and 'libsio.lite-c++' or 'sio.lite-c++.1.0'
    conf.env.append_unique('LIB_SIX_SAMPLES', sioLib)
    conf.env.append_value('CPPPATH_SIX_SAMPLES', os.path.join(conf.path.abspath(), 'sio.lite', 'include'))
    conf.env.append_value('LIBPATH_SIX_SAMPLES', os.path.join(conf.path.abspath(), 'sio.lite'))
    
def build(bld):
    #Options.options.libs_only = True
    bld.add_group()
    bld.add_group()
    bld.add_subdirs('modules projects')

def distclean(context):
    context.recurse('modules projects')
    Scripting.distclean(context)

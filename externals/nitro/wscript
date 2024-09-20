import os
from build import CPPOptionsContext
from waflib import Scripting, Options
from waflib.Build import BuildContext
from waflib.Tools import waf_unit_test

APPNAME = 'nitro'
top     = '.'
out     = 'target'

TOOLS = 'build swig pythontool'
DIRS = 'externals modules'

def options(opt):
    opt.load(TOOLS + ' msvs dumpenv', tooldir='build')
    opt.add_option('--release', action='store_true', dest='release',
                   help='Configure release', default=False)
    opt.recurse(DIRS)

def configure(conf):
    conf.env['APPNAME'] = APPNAME
    conf.load(TOOLS, tooldir='build')

    if conf.env['COMPILER_CXX'] != 'msvc':
        conf.env.CFLAGS += ['-Wall'];
        conf.env.CXXFLAGS += ['-Wall'];

    conf.recurse(DIRS)

def build(bld):
    bld.launch_dir = os.path.join(bld.launch_dir, 'modules')
    bld.recurse(DIRS)

    dct = {'INSTALL':bld.env['PREFIX'], 'VERSION':bld.env['VERSION'], 'PLATFORM':bld.env['PLATFORM']}
    bld(features='subst', dct=dct, source='utils/installer/nitro_installer.iss.in',
        target=bld.path.find_or_declare('installer/nitro_installer.iss'), name='installer')
    bld.install_files('target/installer', bld.path.ant_glob(['utils/installer/*.bmp','utils/installer/*.ico']))
    bld.add_post_fun(waf_unit_test.summary)

def distclean(context):
    context.recurse(DIRS)
    Scripting.distclean(context)

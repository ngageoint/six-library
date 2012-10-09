import os
from waflib import Scripting, Options
from waflib.Build import BuildContext
from waflib.Tools import waf_unit_test

VERSION = '2.7-dev'
APPNAME = 'nitro'
top     = '.'
out     = 'target'

TOOLS = 'build swig javatool pythontool matlabtool'
DIRS = 'c c++ java python mex'
#external

def options(opt):
    opt.load(TOOLS, tooldir='build')
    opt.recurse(DIRS)

def configure(conf):

    conf.env['APPNAME'] = APPNAME
    conf.env['VERSION'] = VERSION
    conf.load(TOOLS, tooldir='build')
    
    conf.recurse(DIRS)

def build(bld):
    bld.recurse(DIRS)
	
    bld(features='install_tgt', install_path='${PREFIX}/installer', dir=bld.path.make_node('utils/installer'), name='installer_resources',
        files=['NITRO.bmp','NITRO.ico','NITRO_small.bmp'])
	
    dct = {'INSTALL':bld.env['PREFIX'], 'VERSION':bld.env['VERSION'], 'PLATFORM':bld.env['PLATFORM']}
    bld(features='subst', dct=dct, source='utils/installer/nitro_installer.iss.in', targets_to_add=['installer_resources'],
        target=bld.path.find_or_declare('installer/nitro_installer.iss'), install_path='${PREFIX}/installer', name='installer')
	
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

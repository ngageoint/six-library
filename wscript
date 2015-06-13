import os
from build import CPPOptionsContext
from waflib import Scripting, Options
from waflib.Build import BuildContext
from waflib.Tools import waf_unit_test

VERSION = '2.7'
APPNAME = 'nitro'
top     = '.'
out     = 'target'

TOOLS = 'build swig javatool pythontool matlabtool'
DIRS = 'externals modules'

def options(opt):
    opt.load(TOOLS + ' msvs dumpenv', tooldir='build')
    opt.add_option('--release', action='store_true', dest='release',
                   help='Configure release', default=False)
    opt.recurse(DIRS)

def configure(conf):

    conf.env['APPNAME'] = APPNAME
    conf.env['VERSION'] = VERSION
    conf.load(TOOLS, tooldir='build')
    conf.recurse(DIRS)

def build(bld):
    bld.recurse(DIRS)

    dct = {'INSTALL':bld.env['PREFIX'], 'VERSION':bld.env['VERSION'], 'PLATFORM':bld.env['PLATFORM']}
    bld(features='subst', dct=dct, source='utils/installer/nitro_installer.iss.in', 
        target=bld.path.find_or_declare('installer/nitro_installer.iss'), name='installer')
    bld.install_files('target/installer', bld.path.ant_glob(['utils/installer/*.bmp','utils/installer/*.ico']))
    bld.add_post_fun(waf_unit_test.summary)

    if not bld.targets:
        bld.targets = ('nrt-unittests,nitf-c-apps,nitro-plugins,nitf-c-tests,'
                       'nitf-c-unittests,cgm-c-tests,nitf-c++-apps,'
                       'nitf-c++-tests,installer')
    if 'HAVE_J2K' in bld.env:
        bld.targets += ',j2k-plugins,j2k-tests'
    if 'HAVE_JPEG' in bld.get_env() :
        bld.targets += ',LibjpegDecompress'

    # TODO: This is a little conservative - some Java modules may not have JNI so
    #       could still build them if we had JAVAC and JAR.  javatool.py does a
    #       better check
    # TODO: waf is not currently building the Java tests
    if bld.env['JAVAC'] and bld.env['JAR'] and bld.is_defined('HAVE_JNI_H'):
        bld.targets += ',cgm-java,nitf-java,nitf.imageio-java,nitf.imagej-java'

    if 'HAVE_MATLAB' in bld.env:
        bld.targets += ',nitf-c-mex,nitf-c++-mex'

    if 'PYTHON' in bld.env and bld.env['PYTHON'] and bld.is_defined('HAVE_PYTHON_H'):
        bld.targets += ',_nitropy'
	
def distclean(context):
    context.recurse(DIRS)
    Scripting.distclean(context)

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

def package(context):
    import glob
    import shutil

    installDir = None
    for subDir in os.listdir(os.getcwd()):
        if os.path.isdir(subDir) and 'bin' in os.listdir(subDir):
            installDir = subDir
            break

    if installDir == None:
        raise Exception('Please run waf install before packaging')

    shutil.copyfile(os.path.join('target', 'settings.config'),
        'settings.config')
    context.to_log('Creating wheel\n')
    subprocess.call(['pip', 'wheel', '.', '--wheel-dir', '.'])
    os.remove('settings.config')
    wheel = glob.glob('pysix*whl')[0]
    numpyWheel = glob.glob('numpy*whl')
    if len(numpyWheel) > 0:
        os.remove(numpyWheel[0])

    context.to_log('Zipping installation\n')
    shutil.copy(wheel, os.path.join(installDir, wheel))
    shutil.make_archive('sixInstall', 'zip', None, installDir)
    os.remove(os.path.join(installDir, wheel))


class Package(Context.Context):
    cmd = 'package'
    fun = 'package'

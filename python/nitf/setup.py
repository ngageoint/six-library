from distutils.core import setup
from distutils.command.install import INSTALL_SCHEMES
import os, glob, sys, shutil

files = [os.path.normpath(f).replace('\\', '/') for f in \
              glob.glob('src/python/*.dll') + \
              glob.glob('src/python/nitf/*.so') + \
              glob.glob('src/python/nitf/*.pyd')]


out_dir = 'build/lib/nitf'
if not os.path.exists(out_dir):
    os.makedirs(out_dir)

for f in files:
    shutil.copy(f, out_dir)


setup(
    name = 'nitro-python',
    version = '2.7-dev',
    url = 'http://nitro-nitf.sourceforge.net',
    author = 'tzellman',
    author_email = 'tzellman@users.sourceforget.net',
    description = 'Python bindings for the NITRO project',
    packages = ['nitf'],
    package_dir = {'':'src/python'},
    license = "LGPL",
    scripts=['src/test/python/nitf_extract.py', 'src/test/python/nitf_print.py'],
)

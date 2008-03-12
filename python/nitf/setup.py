from distutils.core import setup
from distutils.command.install import INSTALL_SCHEMES
import os, glob, sys, shutil

files = [os.path.normpath(f).replace('\\', '/') for f in \
              glob.glob('src/main/python/*.dll') + \
              glob.glob('src/main/python/nitf/*.so') + \
              glob.glob('src/main/python/nitf/*.pyd')]


out_dir = 'build/lib/nitf'
if not os.path.exists(out_dir):
    os.makedirs(out_dir)

for f in files:
    shutil.copy(f, out_dir)


setup(
    name = 'nitro-python',
    version = '1.5-rc6'
    url = 'http://nitro-nitf.sourceforge.net',
    author = 'tzellman',
    author_email = 'tzellman@users.sourceforget.net',
    description = 'Python bindings for the NITRO project',
    packages = ['nitf'],
    package_dir = {'':'src/main/python'},
    license = "LGPL",
    scripts=['src/test/python/nitf_extract.py', 'src/test/python/nitf_print.py'],
)

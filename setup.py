from setuptools import setup, Distribution
import sys
if sys.version_info[0] == 3:
    from configparser import SafeConfigParser
else:
    from ConfigParser import SafeConfigParser
import glob
import os
import platform
import sys

installDir = ''
for child in os.listdir(os.getcwd()):
    if os.path.isdir(child):
        subdirs = os.listdir(child)
        if 'tests' in subdirs:
            installDir = child

if not os.path.exists('settings.config'):
    raise IOError('File settings.config does not exist')

config = SafeConfigParser()
config.read('settings.config')
version = config.get('DEFAULT', 'version')

packages = ''
if platform.system() == 'Windows':
    packages = os.path.join(installDir, 'lib', 'site-packages')
else:
    versionedPython = 'python{0}.{1}'.format(
        sys.version_info[0], sys.version_info[1])
    packages = os.path.join(
        installDir, 'lib', versionedPython, 'site-packages')


class BinaryDistribution(Distribution):
    def is_pure(self):
        return False

codaPyds = glob.glob(os.path.join(packages, 'coda', '_*'))
codaPyds = [os.path.basename(pyd) for pyd in codaPyds]

sixPyds = glob.glob(os.path.join(packages, 'pysix', '_*'))
sixPyds = [os.path.basename(pyd) for pyd in sixPyds]


setup(name = 'pysix',
      version = version,
      description = ('The Sensor Independent XML library (six), is a cross-'
          'platform C++ API for reading and writing NGA\'s complex and '
          'derived sensor independent radar formats. Pysix is a Python '
          'wrapper to allow easy reading and writing of the complex format.'),
      install_requires = [ 'numpy' ],
      package_dir = {'': packages},
      packages = ['pysix', 'coda'],
      package_data = {
          'pysix': sixPyds,
          'coda': codaPyds
      },
      distclass = BinaryDistribution)


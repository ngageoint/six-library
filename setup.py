from setuptools import setup, Distribution
import glob
import os
import platform
import sys

installDir = ''
for child in os.listdir(os.getcwd()):
    if os.path.isdir(child):
        subdirs = os.listdir(child)
        if 'tests' in subdirs and 'bin' in subdirs:
            installDir = child

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

codaPyds = glob.glob(os.path.join(packages, 'coda', '_*pyd'))
codaPyds = [os.path.basename(pyd) for pyd in codaPyds]

sixPyds = glob.glob(os.path.join(packages, 'pysix', '_*pyd'))
sixPyds = [os.path.basename(pyd) for pyd in sixPyds]


setup(name='pysix',
      version='2.2.1',
      description = str(codaPyds),
      package_dir = {'': packages},
      packages=['pysix', 'coda'],
      package_data= {
          'pysix': sixPyds,
          'coda': codaPyds
      },
      distclass=BinaryDistribution)


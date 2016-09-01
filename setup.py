from setuptools import setup, Distribution
import glob
import os
import platform

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
    packages = glob.glob(os.path.join(
        installDir, 'lib', 'python*', 'site-packages'))[0]


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

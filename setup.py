from setuptools import setup, Distribution
import glob
import os
import platform

packages = ''
if platform.system() == 'Windows':
    packages = os.path.join('install', 'lib', 'site-packages')
else:
    packages = glob.glob(os.path.join(
        'install', 'lib', 'python*', 'site-pacakges'))[0]


class BinaryDistribution(Distribution):
    def is_pure(self):
        return False

codaPyds = glob.glob(os.path.join(packages, 'coda', '_*pyd'))
codaPyds = [os.path.basename(pyd) for pyd in codaPyds]

sixPyds = glob.glob(os.path.join(packages, 'pysix', '_*pyd'))
sixPyds = [os.path.basename(pyd) for pyd in sixPyds]

#package_dir = {'': packages},

setup(name='pysix',
      version='2.2.1',
      description = str(codaPyds),
      packages=['pysix', 'coda'],
      package_data= {
          'pysix': sixPyds,
          'coda': codaPyds
      },
      distclass=BinaryDistribution)

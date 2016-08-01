#!/usr/bin/env python

#
# =========================================================================
# This file is part of six.sicd-python
# =========================================================================
#
# (C) Copyright 2004 - 2016, MDA Information Systems LLC
#
# six.sicd-python is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this program; If not,
# see <http://www.gnu.org/licenses/>.
#

import os
import platform
import subprocess
import sys

import makeRegressionFiles
import runMiscTests
import runPythonScripts
import runUnitTests
import checkNITFs
import utils

# If we don't run this before setting the paths, we won't be testing the right
# things
cropSicds = utils.executableName(
        os.path.join(utils.installPath(), 'bin', 'crop_sicd'))
success = subprocess.call([cropSicds, '--start-row', '0', '--start-col', '0',
                           '--num-rows', '10', '--num-cols', '10',
                           os.path.join(utils.sicdDir(),
                                        os.listdir(utils.sicdDir())[0]),
                           'cropped.nitf'], stdout=subprocess.PIPE)

if os.path.exists('cropped.nitf'):
    os.remove('cropped.nitf')                   
if success != 0:
    print("Error running crop_sicd")
    sys.exit(1)
    

utils.setPaths()

if platform.system() != 'SunOS':
    if makeRegressionFiles.run() == False:
        print("Error generating regression files")
        sys.exit(1)

    if runPythonScripts.run() == False:
        print("Error running a python script")
        sys.exit(1)

    if checkNITFs.run() == False:
        print("test in checkNITFS.py failed")
        sys.exit(1)

    if runMiscTests.run() == False:
    # Tests should report their own errors
        sys.exit(1)
else:
    print('Warning: skipping the bulk of the test suite, as Python modules ' +
          'are by default disabled on Solaris')

print("Performing byte swap test")
if subprocess.call([utils.executableName(os.path.join(
        utils.installPath(), 'tests', 'six.sidd', 'test_byte_swap'))]) != 0:
    print("Failed ByteSwap test in six.sidd/tests/test_byte_swap")
    sys.exit(1)
print("Byte swap test succeeded")

if runUnitTests.run() == False:
    print("Unit tests failed")
    sys.exit(1)

print("All passed")
sys.exit(0)


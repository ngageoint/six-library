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

from runner import CppTestRunner


def clean(files):
    for pathname in files:
        if os.path.exists(pathname):
            os.remove(pathname)


def run(sourceDir):
    # If we don't run this before setting the paths, we won't be testing
    # the right things

    sicdDir = os.path.join(sourceDir, 'SICD')
    siddDir = os.path.join(sourceDir, 'SIDD')

    if sourceDir != '':
        os.environ["PATH"] = (os.environ["PATH"] + os.pathsep +
                os.path.join(utils.installPath(), 'bin'))
        cropSicds = utils.executableName('crop_sicd')

        sicdPathname = os.path.join(sicdDir, os.listdir(sicdDir)[0])

        success = subprocess.call([cropSicds,
                '--start-row', '0', '--start-col', '0',
                '--num-rows', '10', '--num-cols', '10',
                sicdPathname, 'cropped.nitf'],
                stdout=subprocess.PIPE)

        print("Running crop_sicd")
        if os.path.exists('cropped.nitf'):
            os.remove('cropped.nitf')
        if success != 0:
            print("Error running crop_sicd")
            return False

    utils.setPaths()

    if platform.system() != 'SunOS':
        if makeRegressionFiles.run() == False:
            print("Error generating regression files")
            return False

        if runPythonScripts.run() == False:
            print("Error running a python script")
            return False

        if checkNITFs.run() == False:
            print("test in checkNITFS.py failed")
            return False

        if runMiscTests.run() == False:
            # These tests should report their own errors
            return False
    else:
        print('Warning: skipping the bulk of the test suite, '
                'since Python modules are by default disabled on Solaris')

    sicdTestDir = os.path.join(utils.installPath(), 'tests', 'six.sicd')
    siddTestDir = os.path.join(utils.installPath(), 'tests', 'six.sidd')
    sampleTestDir = os.path.join(utils.installPath(), 'bin')

    newFiles = utils.installVts()

    sicdTestRunner = CppTestRunner(sicdTestDir)
    siddTestRunner = CppTestRunner(siddTestDir)
    sampleTestRunner = CppTestRunner(sampleTestDir)

    if os.path.exists(sicdDir) and os.path.exists(siddDir):
        if not sicdTestRunner.run('test_streaming_write'):
            return False

        for nitf in os.listdir(sicdDir):
            nitf = os.path.join(sicdDir, nitf)
            if not sampleTestRunner.run('test_read_nitf_from_vts', nitf,
                    'out.nitf'):
                clean(newFiles)
                return False

        for nitf in os.listdir(siddDir):
            nitf = os.path.join(siddDir, nitf)
            if not sampleTestRunner.run('test_read_nitf_from_vts', nitf,
                    'out.nitf'):
                clean(newFiles)
                return False

        clean(newFiles)

        if not siddTestRunner.run('test_byte_swap'):
            return False

    if runUnitTests.run() == False:
        print("Unit tests failed")
        return False

    print("All passed")
    return True

if __name__ == '__main__':
    sicdDir = ''
    if len(sys.argv) > 1:
        sicdDir = sys.argv[1]
    if run(sicdDir):
        sys.exit(0)
    sys.exit(1)


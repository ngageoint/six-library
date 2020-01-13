#!/usr/bin/env python

#
# =========================================================================
# This file is part of six.sicd-python
# =========================================================================
#
# (C) Copyright 2004 - 2019, MDA Information Systems LLC
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


def runCsmTests():
    pathKey = None
    if platform.system() == 'Windows':
        pathKey = 'PATH'
    else:
        pathKey = 'LD_LIBRARY_PATH'
    os.environ[pathKey] = (os.environ[pathKey] + os.pathsep +
                           os.path.join(utils.installPath(), 'lib'))

    nitfPathname = os.path.join(utils.findSixHome(), 'croppedNitfs')
    sicdPathname = os.path.join(nitfPathname, 'SICD')
    siddPathname = os.path.join(nitfPathname, 'SIDD')
    testRunner = CppTestRunner(os.path.join(utils.installPath(), 'bin'))

    if os.path.exists(sicdPathname):
        for sicd in os.listdir(sicdPathname):
            sicd = os.path.join(sicdPathname, sicd)
            if not testRunner.run('test_sicd_csm', sicd):
                return False

    if os.path.exists(siddPathname):
        for sidd in os.listdir(siddPathname):
            sidd = os.path.join(siddPathname, sidd)
            if not testRunner.run('test_sidd_csm', sidd):
                return False
    return True


def runCPHDTests(cphdDir):

    # Directory of CPHD1.0 schemas
    cphdSchemaDir = os.path.join(utils.findSixHome(), 'six',
                                 'modules', 'c++', 'cphd', 'conf', 'schema')

    # Init executable path
    os.environ['PATH'] = (os.environ['PATH'] + os.pathsep +
                          os.path.join(utils.installPath(), 'tests', 'cphd'))

    result = True
    # Run cphd test for each CPHD version
    for file in os.listdir(cphdDir):
        cphdPathname = os.path.join(cphdDir, file)

        # Generate CPHD1.0 output file
        writeCphd = utils.executableName('test_round_trip')
        success = subprocess.call([writeCphd,
                                   cphdPathname, 'output.cphd', cphdSchemaDir],
                                  stdout=subprocess.PIPE)

        print('Running test_round_trip')

        if success != 0:
            if os.path.exists('output.cphd'):
                os.remove('output.cphd')
            print('Error running test_round_trip')
            return False

        # Check if CPHD1.0 input and output files match
        cphdRunner = CppTestRunner(os.path.join(utils.installPath(), 'tests', 'cphd'))
        result = result and cphdRunner.run('test_compare_cphd', cphdPathname, 'output.cphd', cphdSchemaDir)

    # Remove output file after done
    if os.path.exists('output.cphd'):
        os.remove('output.cphd')

    return result


def run(sourceDir):
    # If we don't run this before setting the paths, we won't be testing
    # the right things
    sicdDir = os.path.join(sourceDir, 'SICD')
    siddDir = os.path.join(sourceDir, 'SIDD')
    cphdDir = os.path.join(sourceDir, 'CPHD')

    if sourceDir != '':
        os.environ['PATH'] = (os.environ['PATH'] + os.pathsep +
                              os.path.join(utils.installPath(), 'bin'))
        cropSicds = utils.executableName('crop_sicd')

        sicdPathname = os.path.join(sicdDir, os.listdir(sicdDir)[0])

        success = subprocess.call([cropSicds,
                                   '--start-row', '0', '--start-col', '0',
                                   '--num-rows', '10', '--num-cols', '10',
                                   sicdPathname, 'cropped.nitf'],
                                  stdout=subprocess.PIPE)

        print('Running crop_sicd')
        if os.path.exists('cropped.nitf'):
            os.remove('cropped.nitf')
        if success != 0:
            print('Error running crop_sicd')
            return False

    utils.setPaths()

    if makeRegressionFiles.run() == False:
        print('Error generating regression files')
        return False

    if runPythonScripts.run() == False:
        print('Error running a python script')
        return False

    if checkNITFs.run() == False:
        print('test in checkNITFS.py failed')
        return False

    if runMiscTests.run() == False:
        # These tests should report their own errors
        return False

    sicdTestDir = os.path.join(utils.installPath(), 'tests', 'six.sicd')
    siddTestDir = os.path.join(utils.installPath(), 'tests', 'six.sidd')
    sampleTestDir = os.path.join(utils.installPath(), 'bin')

    sicdTestRunner = CppTestRunner(sicdTestDir)
    siddTestRunner = CppTestRunner(siddTestDir)
    sampleTestRunner = CppTestRunner(sampleTestDir)

    if os.path.exists(sicdDir) and os.path.exists(siddDir):
        sampleSicd = os.path.join(sicdDir, os.listdir(sicdDir)[0])
        sampleSidd = os.path.join(siddDir, os.listdir(siddDir)[0])
        if not sicdTestRunner.run('test_load_from_input_stream', sampleSicd):
            return False

        if not sicdTestRunner.run('test_streaming_write'):
            return False

        if not runCsmTests():
            return False

        if not (siddTestRunner.run('test_byte_swap') and
                siddTestRunner.run('test_geotiff') and
                siddTestRunner.run('test_check_blocking', sampleSidd) and
                siddTestRunner.run('test_sidd_blocking', utils.installPath())):
            return False

        if not sampleTestRunner.run('test_large_offset'):
            return False

    if os.path.exists(cphdDir):
        if not runCPHDTests(cphdDir):
            return False

    if not runUnitTests.run():
        print('Unit tests failed')
        return False

    print('All passed')
    return True

if __name__ == '__main__':
    sourceDir = ''
    if len(sys.argv) > 1:
        sourceDir = sys.argv[1]
    if run(sourceDir):
        sys.exit(0)
    sys.exit(1)

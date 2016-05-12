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
import subprocess
import sys
from subprocess import call

import utils


def sicdToSIO(testsDir):
    print('Running sicd_to_sio.py')
    scriptName = os.path.join(testsDir, 'sicd_to_sio.py')
    sampleNITF = os.path.join(utils.findSixHome(), 'regression_files',
            'six.sicd', 'sicd_1.0.0(RMA)RMAT.nitf')
    schemaPath = os.path.join(utils.findSixHome(), 'install', 'conf',
            'schema', 'six')
    print(schemaPath)
    result = call(['python', scriptName, sampleNITF, schemaPath],
                  stdout=subprocess.PIPE)
    if result == 0:
        os.remove(sampleNITF.rstrip('.nitf') + '.sio')
        print("sicd_to_sio.py succeeded")
        return True
    return False

def testCreateSICDXML(testsDir):
    print('Running test_create_sicd_xml.py')
    scriptName = os.path.join(testsDir, 'test_create_sicd_xml.py')
    result = call(['python', scriptName, '-v', '1.1.0'], stdout=subprocess.PIPE)
    if result == 0:
        os.remove('test_create_sicd.xml')
        os.remove('test_create_sicd_rt.xml')
        print('test_create_sicd_xml.py succeeded')
        return True
    return False

def testSixSICD(testsDir):
    print('Running test_six_sicd.py')
    scriptName = os.path.join(testsDir, 'test_six_sicd.py')
    sampleNITF = os.path.join(utils.findSixHome(), 'regression_files',
            'six.sicd', 'sicd_1.0.0(RMA)RMAT.nitf')
    result = call(['python', scriptName, sampleNITF],
                  stdout = subprocess.PIPE)
    if result == 0:
        os.remove('from_sicd_sicd_1.0.0(RMA)RMAT.nitf.xml')
        print('test_six_sicd.py succeeded')
        return True
    return False

def run():
    testsDir = os.path.join(utils.findSixHome(), 'six',
            'modules', 'python', 'six.sicd', 'tests')

    result = (sicdToSIO(testsDir) and testCreateSICDXML(testsDir) and
            testSixSICD(testsDir))
    return result


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
import tempfile

import utils
from runner import PythonTestRunner


def createSampleCPHD():
    programPathname = os.path.join(utils.installPath(), 'tests',
            'cphd', 'test_cphd_write_simple')
    if not os.path.exists(programPathname):
        programPathname += '.exe'
    if not os.path.exists(programPathname):
        raise IOError('Unable to find ' + programPathname)
    _, cphdPathname = tempfile.mkstemp()
    os.remove(cphdPathname)
    success = subprocess.check_call([programPathname, cphdPathname])
    if success != 0:
        raise OSError('An error occured while executing ' + programPathname)
    return cphdPathname


def run():
    # SICD tests
    testsDir = os.path.join(utils.findSixHome(), 'six',
            'modules', 'python', 'six.sicd', 'tests')
    sampleNITF = os.path.join(utils.findSixHome(), 'regression_files',
            'six.sicd', 'sicd_1.2.0(RMA)RMAT.nitf')
    schemaPath = os.path.join(utils.installPath(), 'conf', 'schema', 'six')

    sicdRunner = PythonTestRunner(testsDir)
    result = (sicdRunner.run('test_streaming_sicd_write.py') and
        sicdRunner.run('test_read_sicd_xml.py', sampleNITF) and
        sicdRunner.run('test_six_sicd.py', sampleNITF) and
        sicdRunner.run('test_create_sicd_xml.py', '-v', '1.2.0') and
        sicdRunner.run('sicd_to_sio.py', sampleNITF, schemaPath) and
        sicdRunner.run('test_read_complex_data.py', sampleNITF) and
        sicdRunner.run('test_simple_round_trip.py', sampleNITF, 'out.xml'))

    # CPHD tests
    cphdPathname = createSampleCPHD()
    testsDir = os.path.join(utils.findSixHome(), 'six',
            'modules', 'python', 'cphd', 'tests')
    cphdRunner = PythonTestRunner(testsDir)
    result = result and cphdRunner.run('test_round_trip_cphd.py', cphdPathname,
            'out.cphd')

    # SIX tests
    testsDir = os.path.join(utils.findSixHome(), 'six',
            'modules', 'python', 'six', 'tests')
    sixRunner = PythonTestRunner(testsDir)
    result = result and sixRunner.run('testDateTime.py');

    return result

if __name__ == '__main__':
    if run():
        sys.exit(0)
    sys.exit(1)


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
import sys

import utils
from runner import PythonTestRunner


def run():
    testsDir = os.path.join(utils.findSixHome(), 'six',
            'modules', 'python', 'six.sicd', 'tests')
    sampleNITF = os.path.join(utils.findSixHome(), 'regression_files',
            'six.sicd', 'sicd_1.2.0(RMA)RMAT.nitf')
    schemaPath = os.path.join(utils.installPath(), 'conf', 'schema', 'six')

    sicdRunner = PythonTestRunner(testsDir)
    result = (sicdRunner.run('test_streaming_sicd_write.py') and
        sicdRunner.run('test_read_region.py') and
        sicdRunner.run('test_read_sicd_xml.py', sampleNITF) and
        sicdRunner.run('test_six_sicd.py', sampleNITF) and
        sicdRunner.run('test_create_sicd_xml.py', '-v', '1.2.0') and
        sicdRunner.run('sicd_to_sio.py', sampleNITF, schemaPath))

    return result

if __name__ == '__main__':
    if run():
        sys.exit(0)
    sys.exit(1)

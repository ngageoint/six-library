#!/user/bin/env/python
#
# =========================================================================
# This file is part of six.sicd-python
# =========================================================================
#
# (C) Copyright 2004 - 2015, MDA Information Systems LLC
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

from pysix.six_sicd import readRegion, read


def createNITF():
    location = os.path.split(os.path.realpath(__file__))[0]
    testPath = os.path.join(location, 'test_create_sicd_xml.py')
    subprocess.call(['python', testPath, '--includeNITF'])
    return os.path.join(os.getcwd(), 'test_create_sicd.nitf')


def clean(pathname):
    basename = os.path.splitext(pathname)
    for extension in ['.nitf', '.xml']:
        os.remove(basename + extension)
        os.remove(basename + '_rt' + extension)


if __name__ == '__main__':
    pathname = createNITF()
    assert os.path.exists(pathname)
    expectedArray, expectedData = read(pathname)
    numRows, numCols = expectedArray.shape
    actualArray, actualData = readRegion(pathname, 0, numRows, 0, numCols)
    try:
        assert (actualArray == expectedArray).all()
        assert actualData == expectedData
    except AssertionError:
        print('Behavior of readRegion() and read() differ. Test failed')
        sys.exit(1)
    except Exception as e:
        sys.exit(repr(e))
    print('Test passed')
    sys.exit(0)


#!/user/bin/env python

"""
* =========================================================================
* This file is part of NITRO
* =========================================================================
*
* (C) Copyright 2004 - 2016, MDA Information Systems LLC
*
* NITRO is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this program; if not, If not,
* see <http://www.gnu.org/licenses/>.
*
*
"""

# This program is meant only to call the various reading functions
# in __init__.py to make sure they don't break.

import os
import sys
from nitf import *

def testHandle(nitf):
    # Print out all the constants to make sure they're defined properly
    print IOHandle.CREATE
    print IOHandle.TRUNCATE
    print IOHandle.OPEN_EXISTING
    print IOHandle.ACCESS_READONLY
    print IOHandle.ACCESS_WRITEONLY
    print IOHandle.ACCESS_READWRITE
    print IOHandle.SEEK_SET
    print IOHandle.SEEK_CUR
    print IOHandle.SEEK_END

    # Call all the reading functions in various configurations
    handleNew = IOHandle('test.nitf', createFlags=IOHandle.OPEN_EXISTING)
    handleNew.read(10)
    handleNew.tell()
    handleNew.getSize()
    handleNew.seek(15)
    handleNew.seek(15, IOHandle.SEEK_SET)
    handleNew.close()

    # And now for writing
    handleWrite = IOHandle('new.nitf',
            accessFlags = IOHandle.ACCESS_WRITEONLY,
            createFlags = IOHandle.CREATE)
    data = '12345'
    handleWrite.write(data, 5)
    handleWrite.close()

    # Read it back in to verify
    handleRead = IOHandle('new.nitf')
    readData = handleRead.read(5)
    assert data == str(readData)

if __name__ == '__main__':
    if len(sys.argv) != 2:
        program = os.path.basename(sys.argv[0])
        usage = 'Usage: {0} <filename>'.format(program)
        sys.exit(usage)

    nitf = sys.argv[1]
    testHandle(nitf)


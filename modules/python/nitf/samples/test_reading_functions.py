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

import logging
import os
import sys
from tempfile import mkstemp

from nitf import *
import numpy as np

logging.basicConfig(level=logging.DEBUG, stream=sys.stdout,
                    format='%(asctime)s %(levelname)s %(message)s')

def testHandle(nitf):
    # Print out all the constants to make sure they're defined properly
    with open(os.devnull, 'w') as sys.stdout:
        print(IOHandle.CREATE)
        print(IOHandle.TRUNCATE)
        print(IOHandle.OPEN_EXISTING)
        print(IOHandle.ACCESS_READONLY)
        print(IOHandle.ACCESS_WRITEONLY)
        print(IOHandle.ACCESS_READWRITE)
        print(IOHandle.SEEK_SET)
        print(IOHandle.SEEK_CUR)
        print(IOHandle.SEEK_END)

    # Call all the reading functions in various configurations
    handleNew = IOHandle(nitf, createFlags=IOHandle.OPEN_EXISTING)
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
    data = [1, 2, 3, 4, 5]
    sizeInBytes = len(data) * 8
    handleWrite.write(data, sizeInBytes, np.dtype('int64'))
    handleWrite.close()

    # Read it back in to verify
    handleRead = IOHandle('new.nitf')
    try:
        readData = handleRead.read(len(data), np.dtype('int64'))
        assert (data == readData).all()
    finally:
        handleRead.close()
        os.remove('new.nitf')


def testSegmentReader():
    data = []
    for i in range(10 * 10):
        data.append(i * i)
    data = np.ascontiguousarray(data, dtype="complex64")

    writer = Writer()
    record = Record()
    graphicSegment = record.newGraphicSegment()
    segmentSource = MemorySegmentSource(data);

    handle, outfile = mkstemp(dir = os.getcwd(), suffix='.ntf')
    os.close(handle)
    outhandle = IOHandle(outfile, IOHandle.ACCESS_WRITEONLY, IOHandle.CREATE)

    writer.prepare(record, outhandle)
    writer.newGraphicWriter(0).attachSource(segmentSource)

    writer.write()
    outhandle.close()

    reader, record = read(outfile)
    readData = reader.newGraphicReader(0).read(data.size, np.dtype("complex64"))
    try:
        assert(data == readData).all()
    finally:
        del reader #This has to delete before we are free to remove the file
        os.remove(outfile)


def testDataSourceReader():
    data = []
    for i in range(100):
        data.append(complex(i, i))
    data = np.ascontiguousarray(data, dtype="complex64")

    bandSource = MemoryBandSource(data, nbpp=3)
    readData = bandSource.read(data.size, np.dtype('complex64'))
    assert(data == readData).all()


if __name__ == '__main__':
    if len(sys.argv) != 2:
        program = os.path.basename(sys.argv[0])
        usage = 'Usage: {0} <filename>'.format(program)
        sys.exit(usage)

    nitf = sys.argv[1]
    testHandle(nitf)
    testSegmentReader()
    testDataSourceReader()


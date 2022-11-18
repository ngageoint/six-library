#!/usr/bin/env python

"""
 * =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
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


import argparse
import os

import nitf


def copyHeader(src, dest):
    for field in src.header.fieldMap:
        dest.header[field] = src.header[field]


def copyImage(reader, image, record):
    segment = record.newImageSegment()
    segment.subheader.fieldMap = image.subheader.fieldMap.copy()
    segment.subheader.ref = image.subheader.ref
    print(dir(segment.subheader))

    print('Metadata copied()')
    print(type(segment.subheader.fieldMap))

    imageSource = nitf.ImageSource()
    window = nitf.SubWindow()
    window.numRows = image.subheader['numRows'].intValue()
    window.numCols = image.subheader['numCols'].intValue()
    window.bandList = list(range(image.subheader.getBandCount()))
    bandData = reader.read(window)
    for ii in window.bandList:
        imageSource.addBand(nitf.MemoryBandSource(bandData, bandData.shape[1],
                                                  ii, 1, bandData.shape[0] - 1))
    print(segment.subheader.fieldMap['pixelvaluetype'])
    return imageSource


def stripSegments(source, output):
    handle = nitf.IOHandle(source)
    reader = nitf.Reader()
    record = reader.read(handle)

    headerLength = int(record.header['HL'])
    numImages = int(record.header['NUMI'])
    print(dir(record.header))
    imageLengths = []
    for ii in xrange(numImages):
        subheaderLength = record.header['LISH{:03d}'.format(ii + 1)]
        imageLength = record.header['LI{:02d}'.format(ii + 1)]
        print(subheaderLength)
    '''
    # imagesOmitted = 0
    sources = []
    for ii, image in enumerate(record.getImages()):
        # if str(image['IREP']).strip() == 'NODISPLY':
            # imagesOmitted += 1
            # continue

        sources.append(
            copyImage(reader.newImageReader(ii), image, outRecord))
    handle = nitf.IOHandle(output, nitf.IOHandle.ACCESS_WRITEONLY)
    copyHeader(record, outRecord)
    writer.prepare(outRecord, handle)
    for ii, source in enumerate(sources):
        imageWriter = writer.newImageWriter(ii)
        imageWriter.attachSource(source)
    writer.write()
    '''


if __name__ == '__main__':
    parser = argparse.ArgumentParser('Round trip a NITF, stripping segments '
                                     'that should not be displayed')
    parser.add_argument('source')
    parser.add_argument('output')
    args = parser.parse_args()
    if not os.path.isfile(args.source):
        raise IOError('{} is not a file'.format(args.source))
    stripSegments(args.source, args.output)


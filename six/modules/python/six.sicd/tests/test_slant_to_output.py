import argparse
import os
import sys

import numpy

import coda.sio_lite as sio_lite
from coda.coda_types import RowColDouble, RowColSizeT, VectorString
import pysix.six_sicd as six_sicd
from pysix.six_sicd import AreaPlaneUtility, SixSicdUtilities


def getSchemaPaths(pathname):
    schemaPaths = VectorString()
    if pathname:
        schemaPaths.push_back(pathname)
    elif 'SIX_SCHEMA_PATH' in os.environ:
        schemaPaths.push_back(os.environ['SIX_SCHEMA_PATH'])
    else:
        schemaPaths.push_back(six_sicd.schema_path())
    return schemaPaths


def findOutputToSlantPolynomials(complexData, orderX, orderY):
    polynomialFitter = SixSicdUtilities.getPolynomialFitter(complexData)
    sampleSpacing = RowColDouble(complexData.grid.row.sampleSpacing,
                                 complexData.grid.col.sampleSpacing)
    offset = RowColSizeT(complexData.imageData.firstRow,
                         complexData.imageData.firstCol)
    scpPixel = RowColDouble(complexData.imageData.scpPixel.row,
                            complexData.imageData.scpPixel.col)

    return polynomialFitter.fitOutputToSlantPolynomials(
        offset,
        scpPixel,
        scpPixel,
        sampleSpacing,
        orderX,
        orderY)


def projectToOutput(inputBuffer, dims, toSlantRow, toSlantCol):
    outputBuffer = numpy.empty((dims.row, dims.col), dtype=numpy.float16)
    toSlantRow = toSlantRow.flipXY()
    toSlantCol = toSlantCol.flipXY()

    for outputRow in range(dims.row):
        rowPoly = toSlantRow.atY(outputRow)
        colPoly = toSlantCol.atY(outputRow)

        for outputCol in range(dims.col):
            inRow = int(round(rowPoly(outputCol)))
            inCol = int(round(colPoly(outputCol)))
            if (inRow < 0 or inRow >= inputBuffer.shape[0] or
                    inCol < 0 or inCol >= inputBuffer.shape[1]):
                outputBuffer[outputRow][outputCol] = 0
            else:
                outputBuffer[outputRow][outputCol] = abs(
                    inputBuffer[inRow][inCol])

    return outputBuffer


def getAreaPlane(complexData):
    if not AreaPlaneUtility.hasAreaPlane(complexData):
        AreaPlaneUtility.setAreaPlane(complexData)
    return complexData.radarCollection.area.plane


if __name__ == '__main__':
    parser = argparse.ArgumentParser('Read a SICD, project the image data to '
                                     'the output plane, and write it as an SIO')
    parser.add_argument('--schema', help='Specifiy a schema or '
                        'directory of schemata')
    parser.add_argument('--polyOrderX', default=3, type=int,
                        help='Order for x-direction polynomials')
    parser.add_argument('--polyOrderY', default=3, type=int,
                        help='Order for y-direction polynomials')
    parser.add_argument('input', help='Input SICD')
    parser.add_argument('output', help='Output SIO')
    args = parser.parse_args()

    if not os.path.isfile(args.input):
        print('{} is not a file'.format(args.input))
        sys.exit(1)

    if args.schema and not os.path.exists(args.schema):
        print('{} does not exist'.format(args.schema))
        sys.exit(1)

    schemaPaths = getSchemaPaths(args.schema)
    widebandData, complexData = six_sicd.read(args.input, schemaPaths)
    plane = getAreaPlane(complexData)
    toSlantRow, toSlantCol = findOutputToSlantPolynomials(
        complexData, args.polyOrderX, args.polyOrderY)
    dims = RowColSizeT(plane.xDirection.elements,
                       plane.yDirection.elements)
    outputBuffer = projectToOutput(
        widebandData,
        dims,
        toSlantRow,
        toSlantCol)

    sio_lite.write(outputBuffer, args.output, numpy.float32)
    sys.exit(0)

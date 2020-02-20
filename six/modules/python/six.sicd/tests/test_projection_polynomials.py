import argparse
import os
import sys

from coda.coda_types import RowColDouble, RowColSizeT, VectorString
from coda.coda_logging import getLogger
import pysix.six_sicd as six_sicd
from pysix.six_sicd import SixSicdUtilities, AreaPlaneUtility


def getSchemaPaths(pathname):
    schemaPaths = VectorString()
    if pathname:
        schemaPaths.push_back(pathname)
    elif 'SIX_SCHEMA_PATH' in os.environ:
        schemaPaths.push_back(os.environ['SIX_SCHEMA_PATH'])
    else:
        schemaPaths.push_back(six_sicd.schema_path())
    return schemaPaths


if __name__ == '__main__':
    parser = argparse.ArgumentParser('Read a SICD, project the image data to '
                                     'the output plane, and write it as an SIO')
    parser.add_argument('--schema', help='Specifiy a schema or '
                        'directory of schemata')
    parser.add_argument('--order', type=int, default=3,
                        help='Order of polynomials to calcualte')
    parser.add_argument('input', help='Input SICD')
    args = parser.parse_args()

    if not os.path.isfile(args.input):
        print('{} is not a file'.format(args.input))
        sys.exit(1)

    if args.schema and not os.path.exists(args.schema):
        print('{} does not exist'.format(args.schema))
        sys.exit(1)

    schemaPaths = getSchemaPaths(args.schema)
    _, complexData = six_sicd.read(args.input, schemaPaths)

    polynomialFitter = SixSicdUtilities.getPolynomialFitter(complexData)
    sampleSpacing = RowColDouble(complexData.grid.row.sampleSpacing,
                                 complexData.grid.col.sampleSpacing)
    offset = RowColSizeT(complexData.imageData.firstRow,
                         complexData.imageData.firstCol)
    scpPixel = RowColDouble(complexData.imageData.scpPixel.row,
                            complexData.imageData.scpPixel.col)

    toSlantRow, toSlantCol = polynomialFitter.fitOutputToSlantPolynomials(
        offset, scpPixel, scpPixel, sampleSpacing, args.order, args.order)

    toOutputRow, toOutputCol = polynomialFitter.fitSlantToOutputPolynomials(
        offset, scpPixel, scpPixel, sampleSpacing, args.order, args.order)

    toSlantRow = toSlantRow.flipXY()
    toSlantCol = toSlantCol.flipXY()

    toOutputRow = toOutputRow.flipXY()
    toOutputCol = toOutputCol.flipXY()

    for row in range(0, complexData.imageData.numRows, 10):
        for col in range(0, complexData.imageData.numCols, 10):
            outputRow = toOutputRow.atY(row)(col)
            outputCol = toOutputCol.atY(row)(col)

            slantRow = toSlantRow.atY(outputRow)(outputCol)
            slantCol = toSlantCol.atY(outputRow)(outputCol)

            if abs(slantRow - row) > 5 or abs(slantCol - col) > 5:
                print('Round trip failed at ({}, {})'.format(row, col))
                print('({}, {}) vs ({}, {})'.format(row, col, slantRow, slantCol))
                sys.exit(1)

    sys.exit(0)


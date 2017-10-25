#!/usr/bin/env python

#
# =========================================================================
# This file is part of six.sicd-python
# =========================================================================
#
# (C) Copyright 2004 - 2017, MDA Information Systems LLC
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

# This is a test file to demonstrate usage of the wrapped GeoLocator class.
# It is used to get location in the output plane from a SICD pixel

import argparse
import os
import sys

from coda.coda_types import RowColDouble
from coda.coda_types import VectorString
from pysix.six_sicd import GeoLocator
from pysix.six_sicd import SixSicdUtilities


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        "Given a SICD and a pixel in the corresponding output plane, "
        "display the lat/lon/alt")
    parser.add_argument("--schemaPath",
                        help="Specify a schema or directory of schemata")
    parser.add_argument("--maintainOrientation", action="store_true",
                        help="Flag to prevent rotation to shadows down")
    parser.add_argument("input", help="Input SICD file")
    parser.add_argument("row", type=float, help="Pixel row location")
    parser.add_argument("col", type=float, help="Pixel col location")
    args = parser.parse_args()

    vs = VectorString()
    if args.schemaPath:
        vs.push_back(args.schemaPath)

    if not os.path.exists(args.input):
        raise IOError('{} does not exist'.format(args.input))
    data = SixSicdUtilities.getComplexData(args.input, vs)
    shadowsDown = not args.maintainOrientation
    locator = GeoLocator(data, shadowsDown)
    lla = locator.geolocate(RowColDouble(args.row, args.col))

    print('Latitude: {}'.format(lla.getLat()))
    print('Longitude: {}'.format(lla.getLon()))
    print('Altitude: {}'.format(lla.getAlt()))
    sys.exit(0)

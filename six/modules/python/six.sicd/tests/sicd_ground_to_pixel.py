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

# Simple program which takes in a SICD and ground point location and provides the
# corresponding pixel row,col in the SICD
# Ground point location can be provided in lat/lon, lat/lon/alt, or ECEF

import sys, os, argparse, pdb
from coda.math_linear import *
from pysix import scene, six_base, six_sicd, sicdUtils

if __name__ == '__main__':
    # Parse the command line
    parser = argparse.ArgumentParser(description=
    """Convert a scene (ground) point to a pixel location in a SICD.  Input can
       be lat/lon (in which case the SCP's altitude will be used), lat/lon/alt,
       or ECEF.""")

    parser.add_argument('--lat', help='Latitude', type=float)
    parser.add_argument('--lon', help='Longitude', type=float)
    parser.add_argument('--alt', help='Altitude in meters', type=float)
    parser.add_argument('--ecef', nargs=3, help='ECEF meters <x y z>', type=float)
    parser.add_argument('--sicd', help='SICD NITF pathname', required=True)
    parser.add_argument('--schema',
        help="""Schema directory.  Required if SIX_SCHEMA_PATH environment
                variable is not set.""")

    args = parser.parse_args()

    # Set the schema paths
    # Try to use the environment variable if we've got it
    schemaPath = os.getenv('SIX_SCHEMA_PATH', args.schema)
    if schemaPath == None:
        sys.exit('If SIX_SCHEMA_PATH environment variable is not set, schema ' +
                 'dir must be passed in')

    schemaPaths = [schemaPath]
    # Read in the XML portion of the SICD
    complexData = sicdUtils.readComplexData(args.sicd, schemaPaths)

    # Derive geometry info from this
    geom = six_sicd.SixSicdUtilities.getSceneGeometry(complexData)
    projModel = six_sicd.SixSicdUtilities.getProjectionModel(complexData, geom)

    # To actually call sceneToImage(), we need ECEF
    # So if they gave us lat/lon, we need to convert
    if args.lat != None:
        alt = args.alt
        if alt == None:
            # We don't have an altitude so use the height at the reference point
            alt = scene.Utilities.ecefToLatLon(geom.getReferencePosition()).getAlt()
            print("Using SCP's height of %f meters" % alt)
        latLonAlt = scene.LatLonAlt(args.lat, args.lon, alt)
        groundPt = scene.Utilities.latLonToECEF(latLonAlt)
    else:
        groundPt = coda.math_linear.Vector3()
        for idx, val in enumerate(args.ecef):
            groundPt[idx] = val

    # Convert to image coordinates
    # This is in meters from the SCP
    imagePt = projModel.sceneToImage(groundPt)

    # Convert to pixel coordinates
    pixelRow = imagePt.row / complexData.grid.row.sampleSpacing + \
            complexData.imageData.scpPixel.row - complexData.imageData.firstRow

    pixelCol = imagePt.col / complexData.grid.col.sampleSpacing + \
            complexData.imageData.scpPixel.col - complexData.imageData.firstCol

    print("Row = %f, Col = %f" % (pixelRow, pixelCol))

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

import sys, os, argparse
from coda.math_linear import *
from coda.coda_types import *
from pysix import scene, six_base, six_sicd

if __name__ == '__main__':
    # Parse the command line
    parser = argparse.ArgumentParser(description=
    """Convert a pixel location in a SICD to a scene (ground) point in
       lat/lon/alt and ECEF""")

    parser.add_argument('--row', help='Image row (pixels)', type=float, required=True)
    parser.add_argument('--col', help='Image col (pixels) ', type=float, required=True)
    parser.add_argument('--alt', help='Altitude (meters)', type=float)
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
    schemaPaths = VectorString()
    schemaPaths.push_back(schemaPath)

    # Read in the XML portion of the SICD
    complexData = six_sicd.SixSicdUtilities.getComplexData(args.sicd, schemaPaths)

    # Derive geometry info from this
    geom = six_sicd.SixSicdUtilities.getSceneGeometry(complexData)
    projModel = six_sicd.SixSicdUtilities.getProjectionModel(complexData, geom)

    # Convert pixel location to meters from SCP
    pixelLoc = coda.coda_types.RowColDouble(args.row, args.col)
    imagePt = complexData.pixelToImagePoint(pixelLoc)

    # Project to the ground plane
    # We do this differently based on if we know the altitude or not
    if args.alt:
        groundPt = projModel.imageToScene(imagePt, args.alt)
    else:
        # Get the ground plane normal
        # If the SICD defines an output plane, we'll base it on this
        # Otherwise we'll use the ETP
        groundPlaneNormal = six_sicd.SixSicdUtilities.getGroundPlaneNormal(complexData)

        groundPt = projModel.imageToScene(imagePt, geom.getReferencePosition(),
                                          groundPlaneNormal)

    print('ECEF (meters): %f %f %f' % (groundPt[0], groundPt[1], groundPt[2]))

    # Convert to lat/lon/alt
    latLonAlt = scene.Utilities.ecefToLatLon(groundPt)
    print('Lat: %f  Lon: %f  Alt (m): %f' %
          (latLonAlt.getLat(), latLonAlt.getLon(), latLonAlt.getAlt()))

#!/usr/bin/env python

#
# =========================================================================
# This file is part of cphd-python 
# =========================================================================
# 
# (C) Copyright 2004 - 2015, MDA Information Systems LLC
#
# cphd-python is free software; you can redistribute it and/or modify
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

# In general, if functionality in CPHD is borrowed from six.sicd,
# refer to six.sicd's test script for more verification

# The numbers used here don't correspond to anything at all

from pysix import cphd
import pysix.six_base as six
from pysix import six_sicd
from pysix import scene
from coda import coda_types
from coda import math_poly
from coda import math_linear

import sys
import multiprocessing

if __name__ == '__main__':
    metadata = cphd.Metadata()

    # Data.h checks
    data = cphd.Data()
    data.sampleType.value = cphd.SampleType.RE32F_IM32F
    data.arraySize.push_back(cphd.ArraySize(2,2))

    #
    # Global.h checks
    #

    glob = cphd.Global()

    # simple stuff first
    glob.domainType.value = cphd.DomainType.TOA
    glob.phaseSGN.value   = cphd.PhaseSGN.MINUS_1
    glob.refFrequencyIndex = 1
    glob.collectionDuration = 0.5

    # LatLonAltCorners for our image area

    lla = scene.LatLonAlt(45,23,1)

    llac = six.LatLonAltCorners()
    llac.upperLeft = scene.LatLonAlt(11,22,1)
    llac.upperRight = scene.LatLonAlt(22,33,2)
    llac.lowerRight = scene.LatLonAlt(33,44,3)
    llac.lowerLeft = scene.LatLonAlt(44,55,4)

    # fill in our imageArea with some stuff

    imArea = cphd.ImageArea()
    imArea.acpCorners = llac
    imArea.plane = cphd.makeScopedCopyableAreaPlane()
   
    # reference point

    refPt = six.ReferencePoint()
    refPt.ecef[0] = 10
    refPt.ecef[1] = 20
    refPt.ecef[2] = 30
    refPt.rowCol.row = 3
    refPt.rowCol.col = 4
    refPt.name = 'name'
    imArea.plane.referencePoint = refPt

    # direction parameters

    imArea.plane.xDirection.unitVector[0] = 1
    imArea.plane.xDirection.unitVector[1] = 0
    imArea.plane.xDirection.unitVector[2] = 0
    imArea.plane.xDirection.spacing = 2
    imArea.plane.xDirection.elements = 3
    imArea.plane.xDirection.first = 4

    imArea.plane.yDirection.unitVector[0] = 0
    imArea.plane.yDirection.unitVector[1] = 1
    imArea.plane.yDirection.unitVector[2] = 0 
    imArea.plane.yDirection.spacing = 2
    imArea.plane.yDirection.elements = 3
    imArea.plane.yDirection.first = 4

    # dwell time parameters

    dt = cphd.makeScopedCopyableDwellTimeParameters()
    dt.codTimePoly = math_poly.Poly2D(1,1)
    dt.codTimePoly[0,0] = 0
    dt.codTimePoly[0,1] = 0
    dt.codTimePoly[1,0] = 0
    dt.codTimePoly[1,1] = 0

    dt.dwellTimePoly = math_poly.Poly2D(1,1)
    dt.dwellTimePoly[0,0] = 0
    dt.dwellTimePoly[0,1] = 0
    dt.dwellTimePoly[1,0] = 0
    dt.dwellTimePoly[1,1] = 0

    imArea.plane.dwellTime = dt

    glob.imageArea = imArea

    metadata._global = glob # i guess 'global' is a keyword

    #
    # Now for SRP.h
    #

    srp = cphd.SRP()
    v3 = math_linear.Vector3()
    v3[1] = 99
    srp.srpPT.push_back(v3)
    pv3 = math_poly.PolyVector3() # aka PolyXYZ
    srp.srpPVTPoly.push_back(pv3)

    metadata.srp = srp

    #
    # Channel.h
    #

    channel = cphd.Channel()
    cp = cphd.ChannelParameters()
    cp.srpIndex = 23
    cp.toaSavedNom = 2
    cp.fxCtrNom = 4
    channel.parameters.push_back(cp)
    
    metadata.channel = channel
  
    #
    # Antenna.h
    #

    metadata.antenna = cphd.makeScopedCopyableCphdAntenna()
    ap = six_sicd.AntennaParameters()
    ap.electricalBoresight = six_sicd.makeScopedCopyableElectricalBoresight()
    ap.electricalBoresight.dcxPoly = math_poly.Poly1D(2)
    ap.electricalBoresight.dcxPoly[1] = 3
    metadata.antenna.twoWay.push_back(ap)

    metadata.collectionInformation.collectorName = "test"
    metadata.collectionInformation.coreName = "test"

    if metadata.isTOA():
        pass
    else:
        print "Wrong domain type"

    numVec = coda_types.VectorSizeT()
    numVec.push_back(100)
    numVec.push_back(100)
    numVec.push_back(100)
    vbm = cphd.VBM(3,
           numVec,
           True,
           True,
           True,
           metadata.getDomainType()
           ) 


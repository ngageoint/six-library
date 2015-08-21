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
    srp.numSRPs = 1
    srp.srpType.value = cphd.SRPType.PVTPOLY
    v3 = math_linear.Vector3()
    v3[0] = 89
    v3[1] = 99
    v3[2] = 109
    srp.srpPT.push_back(v3)
    srp.srpPT.push_back(v3)

    pv3 = math_poly.PolyVector3(1) # aka PolyXYZ
    pv3[0][0] = 3
    pv3[0][1] = 4
    pv3[0][2] = 5
    pv3[1][0] = 6
    pv3[1][1] = 7
    pv3[1][2] = 8
    srp.srpPVTPoly.push_back(pv3)

    srp.srpPVVPoly.push_back(pv3)

    metadata.srp = srp

    #
    # Channel.h
    #

    channel = cphd.Channel()
    cp = cphd.ChannelParameters()
    cp.srpIndex = 23
    cp.nomTOARateSF = 30
    cp.bwSavedNom = 33
    cp.toaSavedNom = 2
    cp.fxCtrNom = 4
    cp.txAntIndex = 7
    cp.rcvAntIndex = 9
    cp.twAntIndex = 13
    channel.parameters.push_back(cp)
    
    metadata.channel = channel
  
    #
    # Antenna.h
    #

    metadata.antenna = cphd.makeScopedCopyableCphdAntenna()
    metadata.antenna.numTxAnt = 0
    metadata.antenna.numRcvAnt = 0
    metadata.antenna.numTWAnt = 2
    for i in range(2):
        ap = six_sicd.AntennaParameters()
        ap.electricalBoresight = six_sicd.makeScopedCopyableElectricalBoresight()
        ap.electricalBoresight.dcxPoly = math_poly.Poly1D(1)
        ap.electricalBoresight.dcxPoly[0] = 2
        ap.electricalBoresight.dcxPoly[1] = 3
        ap.electricalBoresight.dcyPoly = math_poly.Poly1D(1)
        ap.electricalBoresight.dcyPoly[0] = 2
        ap.electricalBoresight.dcyPoly[1] = i
        ap.frequencyZero = 33
        ap.xAxisPoly = math_poly.PolyVector3(1)
        ap.xAxisPoly[0][0] = 3
        ap.xAxisPoly[0][1] = 4
        ap.xAxisPoly[0][2] = 5
        ap.xAxisPoly[1][0] = 6
        ap.xAxisPoly[1][1] = 7
        ap.xAxisPoly[1][2] = 8        
        ap.yAxisPoly = math_poly.PolyVector3(1)       
        ap.yAxisPoly[0][0] = 3                        
        ap.yAxisPoly[0][1] = 4                        
        ap.yAxisPoly[0][2] = 5                        
        ap.yAxisPoly[1][0] = 6                        
        ap.yAxisPoly[1][1] = 7                        
        ap.yAxisPoly[1][2] = 8                        
        ap.halfPowerBeamwidths = six_sicd.makeScopedCopyableHalfPowerBeamwidths()
        ap.halfPowerBeamwidths.dcx = 3
        ap.halfPowerBeamwidths.dcy = i
        ap.array = six_sicd.makeScopedCopyableGainAndPhasePolys()
        ap.array.gainPoly = math_poly.Poly2D(1,1)
        ap.array.gainPoly[0,0] = i
        ap.array.gainPoly[0,1] = i
        ap.array.gainPoly[1,0] = i
        ap.array.gainPoly[1,1] = i 
        ap.array.phasePoly = math_poly.Poly2D(1,1)
        ap.array.phasePoly[0,0] = i
        ap.array.phasePoly[0,1] = i
        ap.array.phasePoly[1,0] = i
        ap.array.phasePoly[1,1] = i

        ap.element = ap.array

        ap.electricalBoresightFrequencyShift.value = six.BooleanType.IS_TRUE
        ap.mainlobeFrequencyDilation.value = six.BooleanType.IS_TRUE
        
        metadata.antenna.twoWay.push_back(ap)

    #
    # Collection information 
    #

    metadata.collectionInformation.collectorName = "test"
    metadata.collectionInformation.coreName = "test"
    metadata.collectionInformation.illuminatorName = "a"
    metadata.collectionInformation.collectType.value = six.CollectType.MONOSTATIC
    metadata.collectionInformation.radarMode.value = six.RadarModeType.STRIPMAP
    metadata.collectionInformation.radarModeID = "something"
    metadata.collectionInformation.classification.level = "UNCLASSIFIED"

    param = six.Parameter()
    param.setName('Number of things seen')
    param.setValue(2)
    metadata.collectionInformation.parameters.push_back(param)

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


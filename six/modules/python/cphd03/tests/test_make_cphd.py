#!/usr/bin/env python

#
# =========================================================================
# This file is part of cphd03-python 
# =========================================================================
# 
# (C) Copyright 2004 - 2015, MDA Information Systems LLC
#
# cphd03-python is free software; you can redistribute it and/or modify
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

from pysix import cphd03
import pysix.six_base as six
from pysix import six_sicd
from pysix import scene
from coda import coda_types
from coda import math_poly
from coda import math_linear

import sys
import multiprocessing

if __name__ == '__main__':
    metadata = cphd03.Metadata()

    # Data.h checks
    data = cphd03.Data()
    data.sampleType.value = cphd03.SampleType.RE32F_IM32F
    data.arraySize.push_back(cphd03.ArraySize(2,2))
    data.arraySize.push_back(cphd03.ArraySize(2,2))
    data.arraySize[0].numVectors = 7
    data.arraySize[0].numSamples = 8
    data.arraySize[1].numVectors = 9
    data.arraySize[1].numSamples = 10
    data.numCPHDChannels = 2
    data.numBytesVBP = 30
    metadata.data = data


    #
    # Global.h checks
    #

    glob = cphd03.Global()

    # simple stuff first
    glob.domainType.value = cphd03.DomainType.TOA
    glob.phaseSGN.value   = cphd03.PhaseSGN.MINUS_1
    glob.refFrequencyIndex = 1
    glob.collectStart = six.DateTime()
    glob.collectionDuration = 200
    glob.txTime1 = 4
    glob.txTime2 = 204

    # LatLonAltCorners for our image area

    lla = scene.LatLonAlt(45,23,1)

    llac = six.LatLonAltCorners()
    llac.upperLeft = scene.LatLonAlt(11,22,1)
    llac.upperRight = scene.LatLonAlt(22,33,2)
    llac.lowerRight = scene.LatLonAlt(33,44,3)
    llac.lowerLeft = scene.LatLonAlt(44,55,4)

    # fill in our imageArea with some stuff

    glob.imageArea = cphd03.ImageArea()
    glob.imageArea.acpCorners = llac
    glob.imageArea.plane = cphd03.makeScopedCopyableAreaPlane()
   
    # reference point

    refPt = six.ReferencePoint()
    refPt.ecef[0] = 10
    refPt.ecef[1] = 20
    refPt.ecef[2] = 30
    refPt.rowCol.row = 3
    refPt.rowCol.col = 4
    refPt.name = 'name'
    glob.imageArea.plane.referencePoint = refPt

    # direction parameters

    glob.imageArea.plane.xDirection.unitVector[0] = 1
    glob.imageArea.plane.xDirection.unitVector[1] = 0
    glob.imageArea.plane.xDirection.unitVector[2] = 0
    glob.imageArea.plane.xDirection.spacing = 2
    glob.imageArea.plane.xDirection.elements = 3
    glob.imageArea.plane.xDirection.first = 4

    glob.imageArea.plane.yDirection.unitVector[0] = 0
    glob.imageArea.plane.yDirection.unitVector[1] = 1
    glob.imageArea.plane.yDirection.unitVector[2] = 0 
    glob.imageArea.plane.yDirection.spacing = 2
    glob.imageArea.plane.yDirection.elements = 3
    glob.imageArea.plane.yDirection.first = 4

    # dwell time parameters

    glob.imageArea.plane.dwellTime = cphd03.makeScopedCopyableDwellTimeParameters()
    glob.imageArea.plane.dwellTime.codTimePoly = math_poly.Poly2D(1,1)
    glob.imageArea.plane.dwellTime.codTimePoly[0,0] = 2
    glob.imageArea.plane.dwellTime.codTimePoly[0,1] = 4
    glob.imageArea.plane.dwellTime.codTimePoly[1,0] = 4
    glob.imageArea.plane.dwellTime.codTimePoly[1,1] = 4

    glob.imageArea.plane.dwellTime.dwellTimePoly = math_poly.Poly2D(1,1)
    glob.imageArea.plane.dwellTime.dwellTimePoly[0,0] = 3
    glob.imageArea.plane.dwellTime.dwellTimePoly[0,1] = 5
    glob.imageArea.plane.dwellTime.dwellTimePoly[1,0] = 5
    glob.imageArea.plane.dwellTime.dwellTimePoly[1,1] = 5

    metadata._global = glob # i guess 'global' is a keyword

    #
    # Now for SRP.h
    #

    srp = cphd03.SRP()
    srp.numSRPs = 1
    srp.srpType.value = cphd03.SRPType.PVTPOLY
    v3 = math_linear.Vector3()
    v3[0] = 89
    v3[1] = 99
    v3[2] = 109
    srp.srpPT.push_back(v3)
    srp.srpPT.push_back(v3)

    pv3 = math_poly.PolyVector3(1) # aka PolyXYZ
    pv3[0] = v3 
    pv3[1] = v3 
    srp.srpPVTPoly.push_back(pv3)

    srp.srpPVVPoly.push_back(pv3)

    metadata.srp = srp

    #
    # Channel.h
    #

    channel = cphd03.Channel()
    cp = cphd03.ChannelParameters()
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

    metadata.antenna = cphd03.makeScopedCopyableCphdAntenna()
    metadata.antenna.numTxAnt = 0
    metadata.antenna.numRcvAnt = 0
    metadata.antenna.numTWAnt = 2
    for i in range(2):
        testV3 = math_linear.Vector3()
        testV3[0] = i+5
        testV3[1] = i+6
        testV3[2] = i+7
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
        ap.xAxisPoly[0] = testV3
        ap.xAxisPoly[1] = testV3
        ap.yAxisPoly = math_poly.PolyVector3(1)       
        ap.yAxisPoly[0] = testV3 
        ap.yAxisPoly[1] = testV3
        ap.halfPowerBeamwidths = six_sicd.makeScopedCopyableHalfPowerBeamwidths()
        ap.halfPowerBeamwidths.dcx = 3
        ap.halfPowerBeamwidths.dcy = i
        ap.array = six_sicd.makeScopedCopyableGainAndPhasePolys()
        ap.array.gainPoly = math_poly.Poly2D(1,1)
        ap.array.gainPoly[0,0] = i+5
        ap.array.gainPoly[0,1] = 2
        ap.array.gainPoly[1,0] = i+5
        ap.array.gainPoly[1,1] = i+5
        ap.array.phasePoly = math_poly.Poly2D(1,1)
        ap.array.phasePoly[0,0] = i+5
        ap.array.phasePoly[0,1] = i+5
        ap.array.phasePoly[1,0] = 3
        ap.array.phasePoly[1,1] = i+5

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




    vecParam = cphd03.VectorParameters()
    vecParam.txTime = 4
    vecParam.txPos = 4
    vecParam.rcvTime = 4
    vecParam.rcvPos = 4
    vecParam.srpTime = 4
    vecParam.srpPos = 4
    vecParam.tropoSRP = 4
    vecParam.ampSF = 4

    # We can't set both of these at the same time
    # Change the comments to switch which one to test
    #vecParam.fxParameters = cphd03.makeScopedCopyableFxParameters()
    #vecParam.fxParameters.Fx0 = 1
    #vecParam.fxParameters.FxSS = 2
    #vecParam.fxParameters.Fx1 = 3
    #vecParam.fxParameters.Fx2 = 3
    vecParam.toaParameters = cphd03.makeScopedCopyableTOAParameters()
    vecParam.toaParameters.deltaTOA0 = 0
    vecParam.toaParameters.toaSS = 3

    metadata.vectorParameters = vecParam

    xml_parser = cphd03.CPHDXMLControl()
    xmlStr1 = xml_parser.toXMLString(metadata)
    newMeta = xml_parser.fromXMLString(xmlStr1)
    xmlStr2 = xml_parser.toXMLString(newMeta)

    if xmlStr1 == xmlStr2:
        print 'Test passed'
    else:
        print 'Test failed'

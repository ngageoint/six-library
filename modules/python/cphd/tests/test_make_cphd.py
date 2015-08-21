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
# refer to six.sicd's test script for verification

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
    m = cphd.Metadata()

    d = cphd.Data()
    d.sampleType.value = cphd.SampleType.RE32F_IM32F

#   TODO
#    d.arraySize

    g = cphd.Global()
    g.domainType.value = cphd.DomainType.TOA
    g.phaseSGN.value   = cphd.PhaseSGN.MINUS_1
    g.refFrequencyIndex = 1
    g.collectionDuration = 0.5

    lla = scene.LatLonAlt()
    lla.setAlt(1)
    lla.setLat(45)
    lla.setLon(23)

    llac = six.LatLonAltCorners()
    llac.lowerLeft = lla

    imArea = cphd.ImageArea()
    imArea.acpCorners = llac
    imArea.plane = cphd.makeScopedCopyableAreaPlane()
    
    refPt = six.ReferencePoint()
    refPt.rowCol.row = 3
    refPt.rowCol.col = 4
    imArea.plane.referencePoint = refPt

    dt = cphd.makeScopedCopyableDwellTimeParameters()
    dt.codTimePoly = math_poly.Poly2D(2,2)
    dt.codTimePoly[1,1] = 2
    imArea.plane.dwellTime = dt

    g.imageArea = imArea
    m._global = g # i guess 'global' is a keyword

    s = cphd.SRP()
    v3 = math_linear.Vector3()
    v3[1] = 99
    s.srpPT.push_back(v3)
    pv3 = math_poly.PolyVector3() # aka PolyXYZ
    s.srpPVTPoly.push_back(pv3)

    m.srp = s

    channel = cphd.Channel()
    cp = cphd.ChannelParameters()
    cp.srpIndex = 23
    cp.toaSavedNom = 2
    cp.fxCtrNom = 4
    channel.parameters.push_back(cp)
    
    m.channel = channel
  
    m.antenna = cphd.makeScopedCopyableCphdAntenna()
    ap = six_sicd.AntennaParameters()
    ap.electricalBoresight = six_sicd.makeScopedCopyableElectricalBoresight()
    ap.electricalBoresight.dcxPoly = math_poly.Poly1D(2)
    ap.electricalBoresight.dcxPoly[1] = 3
    m.antenna.twoWay.push_back(ap)

    m.collectionInformation.collectorName = "test"
    m.collectionInformation.coreName = "test"

    if m.isTOA():
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
           m.getDomainType()
           ) 


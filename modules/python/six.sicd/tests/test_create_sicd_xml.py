#!/usr/bin/env python

#
# =========================================================================
# This file is part of six.sicd-python 
# =========================================================================
# 
# (C) Copyright 2004 - 2015, MDA Information Systems LLC
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

import os
from pysix.scene import *
from pysix.six_sicd import *
from pysix.six_base import *
from coda.xml_lite import *
from coda.coda_io import *
from coda.coda_types import *
from coda.math_poly import *
from coda.math_linear import *

import pdb

cmplx = ComplexData()

### Collection Information ###
collectionInfo = makeScopedCloneableCollectionInformation()
collectionInfo.collectorName = 'Some collector'
collectionInfo.illuminatorName = 'Some illuminator'
collectionInfo.coreName = 'Some corename'
collectionInfo.collectType = CollectType('MONOSTATIC')
collectionInfo.radarMode = RadarModeType('SPOTLIGHT')
collectionInfo.radarModeID = 'Some ID'
collectionInfo.classification.level = 'UNCLASSIFIED'
collectionInfo.countryCodes.push_back('US')

param = Parameter()
param.setName('Collection Param')
param.setValue('334')
collectionInfo.parameters.push_back(param)
param.setValue('335')
collectionInfo.parameters.push_back(param)

cmplx.collectionInformation = collectionInfo

### Image Creation ###
imageCreation = makeScopedCloneableImageCreation()
imageCreation.application = 'Some application'
imageCreation.dateTime = DateTime()
imageCreation.site = 'Some site'
imageCreation.profile = 'Some profile'

cmplx.imageCreation = imageCreation

### Image Data ###
imageData = makeScopedCloneableImageData()
imageData.pixelType = PixelType('RE32F_IM32F')
# TODO: Fill in amplitudeTable
imageData.numRows = 123
imageData.numCols = 456
imageData.firstRow = 9
imageData.firstCol = 16
imageData.fullImage.row = 1000
imageData.fullImage.col = 2000
imageData.scpPixel.row = 500
imageData.scpPixel.col = 1000
imageData.validData.push_back(RowColInt(10, 20))
imageData.validData.push_back(RowColInt(30, 40))
imageData.validData.push_back(RowColInt(50, 60))

cmplx.imageData = imageData

### GeoData ###
geoData = makeScopedCloneableGeoData()
geoData.earthModel = EarthModelType('WGS84')
geoData.scp.ecf[0] = 100
geoData.scp.ecf[1] = 200
geoData.scp.ecf[2] = 300
geoData.scp.llh = LatLonAlt(10, 20, 30)

geoData.imageCorners.upperLeft = LatLon(11, 22)
geoData.imageCorners.upperRight = LatLon(22, 33)
geoData.imageCorners.lowerRight = LatLon(33, 44)
geoData.imageCorners.lowerLeft = LatLon(44, 55)
for i in range(4):
    geoData.validData.push_back(LatLon(i * 11, i * 11 + 13))

geoInfo = makeScopedCloneableGeoInfo()
geoInfo.name = 'My name'
param.setName('Some GeoData param')
param.setValue('Some GeoData value')
geoInfo.desc.push_back(param)
geoInfo.geometryLatLon.push_back(LatLon(10, 20))
geoInfo.geometryLatLon.push_back(LatLon(30, 40))
geoInfo.geometryLatLon.push_back(LatLon(50, 60))
geoData.geoInfos.push_back(geoInfo)

cmplx.geoData = geoData

### Grid ###
grid = makeScopedCloneableGrid()
grid.imagePlane = ComplexImagePlaneType('SLANT')
grid.type = ComplexImageGridType('RGAZIM')
grid.timeCOAPoly = Poly2D(3, 3)
for i in range(4):
    for j in range(4):
        grid.timeCOAPoly[(i, j)] = i + j

grid.row = makeScopedCloneableDirectionParameters()
for i in range(3):
    grid.row.unitVector[i] = i * 10
grid.row.sampleSpacing = 5
grid.row.impulseResponseWidth = 10
grid.row.sign = FFTSign('NEG')
grid.row.impulseResponseBandwidth = 13
grid.row.kCenter = 5
grid.row.deltaK1 = 9
grid.row.deltaK2 = 13
grid.row.deltaKCOAPoly = Poly2D(3, 3)
for i in range(4):
    for j in range(4):
        grid.row.deltaKCOAPoly[(i, j)] = i + j
grid.row.weightType = makeScopedCopyableWeightType()
grid.row.weightType.windowName = 'UNIFORM'

grid.col = makeScopedCloneableDirectionParameters()
for i in range(3):
    grid.col.unitVector[i] = i * 10
grid.col.sampleSpacing = 5
grid.col.impulseResponseWidth = 10
grid.col.sign = FFTSign('NEG')
grid.col.impulseResponseBandwidth = 13
grid.col.kCenter = 5
grid.col.deltaK1 = 9
grid.col.deltaK2 = 13
grid.col.deltaKCOAPoly = Poly2D(3, 3)
for i in range(4):
    for j in range(4):
        grid.col.deltaKCOAPoly[(i, j)] = i + j
grid.col.weightType = makeScopedCopyableWeightType()
grid.col.weightType.windowName = 'UNIFORM'

cmplx.grid = grid

### Timeline ###
timeline = makeScopedCopyableTimeline()
timeline.collectStart = DateTime()
timeline.collectDuration = 5
timeline.interPulsePeriod = makeScopedCopyableInterPulsePeriod()

timelineSet = TimelineSet()
timelineSet.tStart = 1
timelineSet.tEnd = 2
timelineSet.interPulsePeriodStart = 3
timelineSet.interPulsePeriodEnd = 4
timelineSet.interPulsePeriodPoly = Poly1D(3)
for i in range(4):
    timelineSet.interPulsePeriodPoly[i] = i * 10

timeline.interPulsePeriod.sets.push_back(timelineSet)

cmplx.timeline = timeline

### Position ###
position = makeScopedCopyablePosition()
position.arpPoly = PolyVector3(3)
position.grpPoly = PolyVector3(3)
position.txAPCPoly = PolyVector3(3)
for i in range(4):
    for j in range(3):
        position.arpPoly[i][j] = i + j
        position.grpPoly[i][j] = i + j
        position.txAPCPoly[i][j] = i + j

position.rcvAPC = makeScopedCopyableRcvAPC()
rcvAPCPoly = PolyVector3(3)
for i in range(4):
    for j in range(3):
        rcvAPCPoly[i][j] = i * 5 + j
position.rcvAPC.rcvAPCPolys.push_back(rcvAPCPoly)

cmplx.position = position

# Now format it as XML
vs = VectorString()
vs.push_back(os.environ['SIX_SCHEMA_PATH'])
  
xml_ctrl = ComplexXMLControl()
fos = FileOutputStream('test_create_sicd.xml')
#out_doc = xml_ctrl.toXML(cmplx, vs)
#root = out_doc.getRootElement()
#root.prettyPrint(fos)

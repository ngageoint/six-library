#!/usr/bin/env python

#
# =========================================================================
# This file is part of scene-python 
# =========================================================================
# 
# (C) Copyright 2004 - 2015, MDA Information Systems LLC
#
# scene-python is free software; you can redistribute it and/or modify
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

#
# Some helper functions
#

def test_assert(description, *args):
  # args is a list of boolean assertions,
  # if any are false then this test failed
  for test in args:
    if not bool(test):
      print("Test FAILED: " + description)
      exit(1)
  print("Test passed: " + description)
 

def is_about(lhs, rhs, abs_tolerance=0.001):
  # floating point "equality" comparison
  return abs(lhs-rhs) < abs_tolerance

#
# imports
#

from math_linear import *
from coda_except import *
from math_poly import *
from scene import *
from coda_types import *

#
# First test
#


initial = LatLonAlt()
initial.setLat(12)
initial.setLon(33)
initial.setAlt(200)

util = Utilities()
ecef = util.latLonToECEF(initial)
result = util.ecefToLatLon(ecef)

test_assert("LatLonAlt->ECEF->LatLonAlt round trip test", 
    is_about(initial.getLat(), result.getLat()), 
    is_about(initial.getLon(), result.getLon()), 
    is_about(initial.getAlt(), result.getAlt()) 
    )

#
# Second test
#

initial = LatLon()
initial.setLat(24)
initial.setLon(3)

ecef = util.latLonToECEF(initial)
result = util.ecefToLatLon(ecef)

test_assert("LatLon->ECEF->LatLonAlt overload round trip test", 
    is_about(initial.getLat(), result.getLat()), 
    is_about(initial.getLon(), result.getLon()), 
    is_about(0,                result.getAlt()) 
    )

#
# setup for following two tests...
#

ref = LatLonAlt()
ref.setLat(55)
ref.setLon(86)
ref.setAlt(99)

pt = Vector3()
pt[0] = 50
pt[1] = 81
pt[2] = 94

#
# Third test
#

ect = ENUCoordinateTransform(ref)
enu_ecef_result  = ect.toECEFPoint(pt)
enu_local_result = ect.toLocalPoint(pt)

# these numbers were taken from a test C++ program
test_assert("ENUCoordinateTransform toECEFPoint, toLocalPoint tests", 
    is_about(enu_ecef_result[0],    255721.850), 
    is_about(enu_ecef_result[1],   3657709.613), 
    is_about(enu_ecef_result[2],   5201588.079), 
    is_about(enu_local_result[0],      -44.227), 
    is_about(enu_local_result[1],    20091.423), 
    is_about(enu_local_result[2], -6363769.244)
    )

#
# Fourth test
#

ned = NEDCoordinateTransform(ref)
ned_ecef_result = ned.toECEFPoint(pt)
ned_local_result = ned.toLocalPoint(pt)

# same for these numbers
test_assert("NEDCoordinateTransform toECEFPoint, toLocalPoint tests", 
    is_about(ned_ecef_result[0],   255685.175),
    is_about(ned_ecef_result[1],  3657629.538),
    is_about(ned_ecef_result[2],  5201416.297),
    is_about(ned_local_result[0],   20091.423),
    is_about(ned_local_result[1],     -44.227),
    is_about(ned_local_result[2], 6363769.244)
    )

#
# Fifth test
#

arpvel = Vector3()
arppos = Vector3()
refpos = Vector3()
imR = Vector3()
imC = Vector3()
opX = Vector3()
opY = Vector3()

arpvel[0] = 0
arpvel[1] = 1
arpvel[2] = 1

arppos[0] = 0
arppos[1] = 0
arppos[2] = 1

refpos[0] = 1
refpos[1] = 1
refpos[2] = 1

imR[0] = 1
imR[1] = 0
imR[2] = 0
imC[0] = 0
imC[1] = 1
imC[2] = 0

opX[0] = 0
opX[1] = 1
opX[2] = 0
opY[0] = 0
opY[1] = 0
opY[2] = 1

sceneGeom = SceneGeometry(arpvel, arppos, refpos, imR, imC, opX, opY)

layover = sceneGeom.getLayover()

test_assert("SceneGeometry tests",
    is_about(sceneGeom.getOPNorthAngle(),     0.00094889, 0.000001),
    is_about(sceneGeom.getOPShadowAngle(),         270.0),
    is_about(sceneGeom.getDopplerConeAngle(),         60),
    is_about(sceneGeom.getOPTiltAngle(),          35.264),
    is_about(layover.angle,                        135.0),
    is_about(layover.magnitude,                  1.41421)
    )

 #              
 # Sixth test   
 #

polarAnglePoly = Poly1D(2)
ksfPoly        = Poly1D(2)
slantPlaneNormal = Vector3()
scp = Vector3()

ap2 = Vector3()
ap2[0] = 3
ap2[1] = 4
ap2[2] = 5

ap1 = Vector3()
ap1[0] = 6
ap1[1] = 7
ap1[2] = 8

ap0 = Vector3()
ap0[0] = 9
ap0[1] = 10
ap0[2] = 11

arppoly = PolyVector3(2)
arppoly[0] = ap0
arppoly[1] = ap1
arppoly[2] = ap2

timeCoaPoly = Poly2D(2,2)
timeCoaPoly[0,0] = 12
timeCoaPoly[0,1] = 13
timeCoaPoly[0,2] = 14
timeCoaPoly[1,0] = 15
timeCoaPoly[1,1] = 16
timeCoaPoly[1,2] = 17
timeCoaPoly[2,0] = 18
timeCoaPoly[2,1] = 19
timeCoaPoly[2,2] = 20

lookDir = 1

polarAnglePoly[0] = 2.2
polarAnglePoly[1] = 3.3
polarAnglePoly[2] = 1

ksfPoly[0] = 2.5
ksfPoly[1] = 3.6
ksfPoly[2] = 1.1

slantPlaneNormal[0] = 3
slantPlaneNormal[1] = 4
slantPlaneNormal[2] = 5

scp[0] = 1
scp[1] = 2
scp[2] = 0.5

rapm = RangeAzimProjectionModel(polarAnglePoly, ksfPoly, slantPlaneNormal,
    imR, imC, scp, arppoly, timeCoaPoly, lookDir)

rowcol = RowColDouble()
rowcol.row = 14
rowcol.col = 32
groundRefPoint = Vector3()
groundPlaneNormal = Vector3()
groundRefPoint[0] = 1
groundRefPoint[1] = 1
groundRefPoint[2] = 0
groundPlaneNormal[0] = 0
groundPlaneNormal[1] = 0
groundPlaneNormal[2] = 1

except_caught = False

try:
  scn = rapm.imageToScene(rowcol, groundRefPoint, groundPlaneNormal)
except:
  except_caught = True


test_assert('Test RangeAzimProjectionModel for \'no solution\'...', except_caught)

#
# Seventh test
#

# we'll reuse arppoly and timeCoaPoly...

imR   = Vector3()
imC   = Vector3()
refPt = Vector3()

imR[0] = 0
imR[1] = 1
imR[2] = 0

imC[0] = 1
imC[1] = 0
imC[2] = 0

refPt[0] = 4
refPt[0] = 4
refPt[0] = 0

pgg = PlanarGridGeometry(imR, imC, refPt, arppoly, timeCoaPoly)

testpt = Vector3()
testpt[0] = 2
testpt[1] = 2
testpt[2] = 2

except_caught = False
try:
  scn_pt = pgg.gridToScene(testpt,1)
  out_pt = pgg.sceneToGrid(scn_pt)
except:
  except_caught = True

test_assert('Test PlanarGridGeometry for \'no solution\'...', except_caught)

print "TODO: real tests for ProjectionModel and GridGeometry"

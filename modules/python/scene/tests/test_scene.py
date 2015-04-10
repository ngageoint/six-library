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

from scene import *

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


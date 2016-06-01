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

# Simple program to show how you can go from XML to and from SIX

from pysix.six_sicd import *
from pysix.six_base import *
from coda.coda_logging import *
import sys, os

# Parse the command line
if len(sys.argv) != 2:
    sys.exit('Usage ' + sys.argv[0] + ' <XML or NITF pathname>')

inPathname = sys.argv[1]

schemaPaths = VectorString()
schemaPaths.push_back(os.environ['SIX_SCHEMA_PATH'])
logger = NullLogger()

# The point of this script is to show we can hop back and forth with
# just XML strings so if they gave us a NITF, grab the XML out of it
# and write it to a text file
gotNITF = inPathname.endswith('.ntf') or inPathname.endswith('.nitf')
if gotNITF:
    xmlPathname = inPathname + '.xml'
    data = SixSicdUtilities.getComplexData(inPathname, schemaPaths)
    xmlStr = SixSicdUtilities.toXMLString(data, schemaPaths, logger)
    file = open(xmlPathname, 'w')
    file.write(xmlStr)
    file.close()
else:
    xmlPathname = inPathname

# Go from XML file on disk to ComplexData
data = SixSicdUtilities.parseDataFromFile(xmlPathname, schemaPaths, logger)
if gotNITF:
    os.remove(xmlPathname)

# Go from ComplexData to string
xmlStr = SixSicdUtilities.toXMLString(data, schemaPaths, logger)
print(xmlStr)

# Print a few things to show that we really parsed this into an object
print("file is " + str(data.getNumRows()) + "x" + str(data.getNumCols()))
print(str(data.getNumBytesPerPixel()) + " bytes per pixel")
print( "tx polarization: " + data.radarCollection.txPolarization.toString())
print("image formation algorithm: " + data.imageFormation.imageFormationAlgorithm.toString())
print("graze angle: " + str(data.scpcoa.grazeAngle))
print("slant range: " + str(data.scpcoa.slantRange))
print("radar mode: " + data.collectionInformation.radarMode.toString())

# Go from string back to ComplexData
data2 = SixSicdUtilities.parseDataFromString(xmlStr, schemaPaths, logger)

# Back to string one last time to compare
xmlStr2 = SixSicdUtilities.toXMLString(data2, schemaPaths, logger)

if xmlStr == xmlStr2:
    print('Strings match')
else:
    sys.exit('Strings do not match!')

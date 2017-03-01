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

from pysix.sicdUtils import *
from pysix.six_sicd import *
from pysix.six_base import *
from coda.xml_lite import *
from coda.coda_io import *
from coda.coda_logging import *

import os
import sys
import os.path
import filecmp

##############################################################
#
# roundTrip test
#
##############################################################

# This test loads up an XML file to a complex data object and
# writes it out again. Then it compares the two files to see if
# they match

def roundTrip(filename):
  cmplx = readComplexData(filename)
  out_filename = 'round_trip_' + os.path.basename(filename)
  writeXML(cmplx, out_filename)
  round_tripped_cmplx = readComplexData(out_filename)

  if cmplx == round_tripped_cmplx:
      print(filename + " passed")
  else:
      print("handling " + filename + " failed")


##############################################################
#
# loadSicd test
#
##############################################################

# This test loads a sicd file as a complex data object and writes
# it out as an xml file. Unfortunately there is no handy way to compare
# the resulting xml so it just prints out various information and assumes
# it worked. Also real sicds can be kind of big so there probably won't
# be any in the repository, so this is really more of an example.

def loadSicd(filename):
  cmplx = readComplexData(filename)
  showInfo(cmplx)
  out_filename = 'from_sicd_' + os.path.basename(filename) + '.xml'
  writeXML(cmplx, out_filename)
  print(filename + " probably passed")
  return cmplx

##############################################################
#
# showInfo test
#
##############################################################

# This isn't a test per se but is useful to put some assorted
# ComplexData member information on the screen just to show
# that we can actually get to the data

def showInfo(cmplx):
  print("file is " + str(cmplx.getNumRows()) + "x" + str(cmplx.getNumCols()))
  print(str(cmplx.getNumBytesPerPixel()) + " bytes per pixel")
  print( "tx polarization: " + cmplx.radarCollection.txPolarization.toString())
  print("image formation algorithm: " + cmplx.imageFormation.imageFormationAlgorithm.toString())
  print("graze angle: " + str(cmplx.scpcoa.grazeAngle))
  print("slant range: " + str(cmplx.scpcoa.slantRange))
  print("radar mode: " + cmplx.collectionInformation.radarMode.toString())
  collectionParameters = cmplx.radarCollection.parameters
  print("radar collection parameters: ")
  if collectionParameters.empty():
    print('    (none)')
  else:
    for idx in range(0,collectionParameters.size()):
      print("    " + collectionParameters[idx].getName() + ": " + str(collectionParameters[idx]))
  print('image data \'validData\'')
  validData = cmplx.imageData.validData
  if validData.empty():
    print('    (none)')
  else:
    for idx in range(0,validData.size()):
      print('    (' + str(validData[idx].row) + ',' + str(validData[idx].col) + ')')


###############################################################################
###############################################################################
###############################################################################
###############################################################################
###############################################################################

if 1 == len(sys.argv):
    print("please provide a sicd or XML file")

for arg in sys.argv:
  if (arg.endswith(".ntf") or arg.endswith(".nitf")):
    cmplx = loadSicd(arg)
  elif arg.endswith(".xml"):
    roundTrip(arg)


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
# loadSicd test
#
##############################################################

# This test loads a sicd file as a complex data object and writes
# it out as an xml file. Unfortunately there is no handy way to compare
# the resulting xml so it just prints out various information and assumes
# it worked. Also real sicds can be kind of big so there probably won't
# be any in the repository, so this is really more of an example.

def loadSicd(filename):
  vs = VectorString()
  vs.push_back( os.environ['SIX_SCHEMA_PATH'] )
  cmplx = SixSicdUtilities.getComplexData(filename, vs)
  return cmplx.validate(getLogger())

if 1 == len(sys.argv):
    print("please provide a sicd or XML file")

for arg in sys.argv:
  if (arg.endswith(".ntf") or arg.endswith(".nitf")):
    cmplx = loadSicd(arg)


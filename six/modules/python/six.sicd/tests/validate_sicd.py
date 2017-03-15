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
from coda.coda_types import *
from coda.coda_logging import *

import os
import sys

##############################################################
#
# validateSicd test
#
##############################################################

# This test loads a sicd file as a complex data object and checks to see
# if it is valid. Discrepancies get written to stdout

def validateSicd(filename):
  vs = VectorString()
  vs.push_back( os.environ['SIX_SCHEMA_PATH'] )
  cmplx = SixSicdUtilities.getComplexData(filename, vs)
  return cmplx.validate(getLogger())

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Usage: {0} <SICD pathname>".format(sys.argv[0]))
        sys.exit(1)
    pathname = sys.argv[1]
    if (pathname.endswith(".ntf") or pathname.endswith(".nitf")):
        if validateSicd(pathname):
            print("SICD is valid")
            sys.exit(0)
        print("SICD is invalid")
    sys.exit(1)


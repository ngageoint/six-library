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
from pysix.sicdUtils import *
from coda.coda_logging import *
from coda.coda_io import *
from coda.xml_lite import *

import os
import sys

# Make sure SixSicdUtilities.getComplexData works on both NITFs and XML

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Usage: {0} <SICD pathname>".format(sys.argv[0]))
        sys.exit(1)
    pathname = sys.argv[1]

    if (pathname.endswith(".nitf") or pathname.endswith(".ntf")):
        complexDataFromNITF = readComplexData(pathname)
        xmlPathname = os.path.basename(os.path.splitext(pathname)[0] + '.xml')
        writeXML(complexDataFromNITF, xmlPathname)
        complexDataFromXML = readComplexData(xmlPathname)

        success = complexDataFromNITF == complexDataFromXML
        if os.path.exists(xmlPathname):
            os.remove(xmlPathname)

        if success:
            sys.exit(0)
        else:
            print('Round-tripped ComplexData did not match')
            sys.exit(1)

    else:
        print('Please input a SICD')

    sys.exit(1)


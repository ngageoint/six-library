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

from pysix.cphd03 import CPHDReader
import sys
import multiprocessing

if __name__ == '__main__':
    if len(sys.argv) >= 2:
        inputPathname = sys.argv[1]
    else:
        print "Usage: " + sys.argv[0] + " <Input CPHD>"
        sys.exit(0)

    reader = CPHDReader(inputPathname, multiprocessing.cpu_count())

    print reader.getFileHeader()
    print reader.getMetadata()
    print reader.getVBM()
    print ''
    print 'VBM Data:'
    print reader.getVBM().toBuffer(0)
    print ''
    print 'Wideband Data:'
    print reader.getWideband().read()



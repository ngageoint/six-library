#!/usr/bin/env python

#
# =========================================================================
# This file is part of six.sicd-python
# =========================================================================
#
# (C) Copyright 2004 - 2019, MDA Information Systems LLC
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

# Simple program to show the use of the cropSICD python bindings

from pysix import six_sicd
from coda import coda_types
import sys, os

# Parse the command line
if len(sys.argv) != 2:
    sys.exit('Usage ' + sys.argv[0] + ' <sicd path name>')


sicd_path_name = sys.argv[1]

base_name = os.path.basename(sicd_path_name)

file_name = os.path.splitext(base_name)[0]
ending  = os.path.splitext(base_name)[1]

output_name = os.path.join('.', file_name + "_cropped" + ending)

# To crop a sicd saved on disk and write it out
# to a new sicd on disk, use cropSICD
six_sicd.cropSICD(sicd_path_name,
                  coda_types.VectorString(),
                  coda_types.RowColSizeT(0,0),
                  coda_types.RowColSizeT(200, 200),
                  output_name)

# If the sicd metadata is already in memory cropMetaData
# can be used 
data, meta_data = six_sicd.read(sicd_path_name)
new_meta_data = six_sicd.cropMetaData(meta_data,
                                      coda_types.RowColSizeT(0,0),
                                      coda_types.RowColSizeT(200, 200))
                  
old_lr = meta_data.geoData.imageCorners.lowerRight
new_lr = new_meta_data.geoData.imageCorners.lowerRight
print ("Old lower Right Corner lat/lon", old_lr.getLat(), old_lr.getLon())
print ("New lower Right Corner lat/lon", new_lr.getLat(), new_lr.getLon())

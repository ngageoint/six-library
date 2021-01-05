#!/usr/bin/env python

#
# =========================================================================
# This file is part of cphd-python
# =========================================================================
#
# (C) Copyright 2004 - 2020, MDA Information Systems LLC
#
# cphd-python is free software; you can redistribute it and/or modify
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

# In general, if functionality in CPHD is borrowed from six.sicd,
# refer to six.sicd's test script for more verification

import os
import sys

import numpy as np

from pysix.cphd import PVPBlock, CPHDWriter, CPHDReader
from coda.coda_types import VectorString

from util import get_test_metadata, get_test_pvp_data, get_test_widebands

NUM_THREADS = 16
SCRATCH_SPACE = 4 * 1024 * 1024

if __name__ == '__main__':

    metadata = get_test_metadata(has_support_array=False, is_compressed=False)
    widebands = get_test_widebands(metadata)
    pvp_block = PVPBlock.fromListOfDicts(get_test_pvp_data(metadata), metadata)

    schema_paths = VectorString()
    schema_paths.push_back(os.environ['SIX_SCHEMA_PATH'])

    cphd_filepath = os.path.join(os.getcwd(), 'test_cphd.nitf')
    cphd_writer = CPHDWriter(metadata, cphd_filepath, schema_paths, NUM_THREADS)

    # writeWideband() writes complete CPHD: XML metadata, PVP data, and wideband data
    cphd_writer.writeWideband(metadata, pvp_block, widebands)

    # Check that we correctly wrote the wideband data
    reader = CPHDReader(cphd_filepath, SCRATCH_SPACE)
    for channel in range(metadata.getNumChannels()):
        if not np.array_equal(reader.getWideband().read(channel=channel), widebands[channel]):
            print('Test failed, original wideband and wideband from file differ in channel {0}'
                  .format(channel))
            sys.exit(1)
        if not np.array_equal(reader.getPHD(channel), widebands[channel]):
            print('Test failed, original wideband and PHD from file differ in channel {0}'
                  .format(channel))
            sys.exit(1)

    os.remove(cphd_filepath)
    print('Test passed')

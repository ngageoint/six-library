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

import sys

import numpy as np

from pysix.cphd import PVPBlock

from util import get_test_metadata, get_test_pvp_data


def main():
    metadata, support_arrays = get_test_metadata(has_support_array=True, is_compressed=True)
    pvp_data = get_test_pvp_data(metadata)  # list of dicts

    if len(pvp_data) != metadata.getNumChannels():
        print('Test failed: expected {0} channels but received {1} from PVPBlock.from_list_of_dicts()'
              .format(metadata.getNumChannels(), len(pvp_data)))
        sys.exit(1)

    pvp_block = PVPBlock.from_list_of_dicts(pvp_data, metadata)
    pvp_data_from_block = pvp_block.to_list_of_dicts(metadata)  # list of dicts

    if len(pvp_data_from_block) != metadata.getNumChannels():
        print('Test failed: expected {0} channels but received {1} from PVPBlock.to_list_of_dicts()'
              .format(metadata.getNumChannels(), len(pvp_data_from_block)))
        sys.exit(1)

    for channel in range(metadata.getNumChannels()):
        if pvp_data[channel].keys() != pvp_data_from_block[channel].keys():
            print('Test failed: list of PVP parameters differs for channel {0}'.format(channel))
            sys.exit(1)
        for param in pvp_data[channel]:
            if not np.array_equal(pvp_data[channel][param], pvp_data_from_block[channel][param]):
                print('Test failed: PVP data differs for parameter {0}, channel {1}'
                      .format(param, channel))
                sys.exit(1)

    print('Test passed')


if __name__ == '__main__':
    main()

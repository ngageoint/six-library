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

from pysix.cphd import CPHDXMLControl

from util import get_test_metadata

if __name__ == '__main__':
    xml_parser = CPHDXMLControl()
    metadata = get_test_metadata(has_support_array=True, is_compressed=True)

    # For an 'Invalid XML' error below, you can use CPHDXMLControl.toXMLString(cphd_metadata)
    # to write the invalid XML to a string and verify that against the CPHD schemas

    try:
        xml_str = xml_parser.toXMLString(metadata)
        meta_from_str = xml_parser.fromXMLString(xml_str)
        xml_str_from_meta_from_str = xml_parser.toXMLString(meta_from_str)

        if xml_str == xml_str_from_meta_from_str:
            print('Test passed')
        else:
            print('Test failed: XML strings differ')
    except Exception as e:
        print('Test failed with exception:', e)
        sys.exit(1)

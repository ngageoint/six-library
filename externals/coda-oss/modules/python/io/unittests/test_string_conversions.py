#!/usr/bin/env python

"""
 * =========================================================================
 * This file is part of io-python
 * =========================================================================
 *
 * (C) Copyright 2019, MDA Information Systems LLC
 *
 * io-python is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 *
"""

import unittest
import sys
from coda.coda_io import StringStream


class TestStringStream(unittest.TestCase):
    def test_write_string(self):
        stream = StringStream()
        stream.write('text')
        self.assertEqual(stream.str(), 'text')

    def test_write_bytes(self):
        stream = StringStream()
        if sys.version_info[0] == 3:
            bytesInput = bytes('text', 'utf-8')
        else:
            # There's no reason to ever want to do this, but just to prove
            # that nothing breaks...
            bytesInput = bytes('text')
        stream.writeBytes(bytesInput)
        self.assertEqual(stream.str(), 'text')


if __name__ == '__main__':
    unittest.main()

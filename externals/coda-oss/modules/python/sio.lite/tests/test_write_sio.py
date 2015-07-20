#!/usr/bin/env python

"""
 * =========================================================================
 * This file is part of sio.lite-python
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * sio.lite-python is free software; you can redistribute it and/or modify
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

from coda.sio_lite import write
import numpy as np
import sys

if __name__ == '__main__':
    if len(sys.argv) >= 3:
        arrayOutputPathname = sys.argv[1]
        transposedOutputPathname = sys.argv[2]
    else:
        print "Usage: " + sys.argv[0] + " <Output SIO for array> <Output SIO for transposed array>"
        sys.exit(0)

    array = np.array([[0.1, 1.1, 2.1, 3.1],
                      [4.2, 5.2, 6.2, 7.2],
                      [8.3, 9.3, 10.3, 11.3]], dtype='float32');
    write(array, arrayOutputPathname)

    #np.transpose() produces a non-contiguous array
    trans = np.transpose(array)
    write(trans, transposedOutputPathname)


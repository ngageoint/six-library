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

from coda.sio_lite import read
import sys

if __name__ == '__main__':
    if len(sys.argv) >= 2:
        inputPathname = sys.argv[1]
    else:
        print("Usage: " + sys.argv[0] + " <Input SIO>")
        sys.exit(0)
        
    array = read(inputPathname)
    
    print("Dims: " + str(array.shape))
    print("Type: " + str(array.dtype))
    

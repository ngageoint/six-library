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

from coda.sio_lite import read, write
import sys

if __name__ == '__main__':

    if len(sys.argv) >= 3:
        inputPathname = sys.argv[1]
        outputPathname = sys.argv[2]
    else:
        print "Usage: " + sys.argv[0] + " <Input SIO> <Output SIO>"
        sys.exit(0)
        
    buf = read(inputPathname)
    
    print 'Input Dims: ' + str(buf.shape)
    print 'Input Type: ' + str(buf.dtype)

    write(buf, outputPathname)
    
    # Read the new one
    buf = read(outputPathname)
    
    print ''
    print 'Output Dims: ' + str(buf.shape)
    print 'Output Type: ' + str(buf.dtype)

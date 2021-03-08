"""
 * =========================================================================
 * This file is part of CODA-OSS 
 * =========================================================================
 *
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
 *
 * CODA-OSS is free software; you can redistribute it and/or modify
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

import coda.numpyutilstest
import numpy

# A simple test to demonstrate basic usage of the numpyutilstest 
# SWIG interface.  This is the python version calling the C implementation
# in source/numpyutilstest.i

print(coda.numpyutilstest.elementDoubleTest.__doc__)

numpyarray = numpy.zeros((2,3), dtype=numpy.float32)
numpyarray[0,0] = 1.0
print("Before Doubling", numpyarray)

numpyarray = coda.numpyutilstest.elementDoubleTest(numpyarray)
print("After Doubling", numpyarray)

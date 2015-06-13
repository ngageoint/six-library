#!/usr/bin/env python

"""
 * =========================================================================
 * This file is part of math.poly-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * math.linear-c++ is free software; you can redistribute it and/or modify
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

import sys
from math_linear import VectorDouble, MatrixDouble
from math_poly import *

if __name__ == '__main__':
    #################
    # Basic 1D test #
    #################

    poly1D = Poly1D(2)
    for x in range(poly1D.order() + 1):
        poly1D[x] = (x + 1) * 10

    print '1D poly:'
    print poly1D

    # Try to index out of bounds by getting
    threw = False
    try:
        foo = poly1D[3]
    except ValueError:
        threw = True

    if threw:
        print 'Getting 1D OOB threw as expected'
    else:
        sys.exit('Getting 1D OOB did not throw!')

    # Try to index out of bounds by setting
    threw = False
    try:
        poly1D[3] = 5
    except ValueError:
        threw = True

    if threw:
        print 'Setting 1D OOB threw as expected'
    else:
        sys.exit('Setting 1D OOB did not throw!')

    #################
    # Basic 2D test #
    #################

    poly2D = Poly2D(2, 3)
    val = 100
    for x in range(poly2D.orderX() + 1):
        for y in range(poly2D.orderY() + 1):
            poly2D[(x, y)] = val
            val += 100

    print '\n2D poly:'
    print poly2D

    # Try to index out of bounds by getting
    threw = False
    try:
        foo = poly2D[(3, 3)]
    except ValueError:
        threw = True

    if threw:
        print 'Getting 2D OOB threw as expected'
    else:
        sys.exit('Getting 2D OOB did not throw!')

    # Try to index out of bounds by setting
    threw = False
    try:
        poly2D[(3, 3)] = 5
    except ValueError:
        threw = True

    if threw:
        print 'Setting 2D OOB threw as expected'
    else:
        sys.exit('Setting 2D OOB did not throw!')

    ############################
    # 1D Fit test (array args) #
    ############################

    xObs = new_doubleArray(4)
    doubleArray_setitem(xObs, 0, 1)
    doubleArray_setitem(xObs, 1, -1)
    doubleArray_setitem(xObs, 2, 2)
    doubleArray_setitem(xObs, 3, -2)

    yObs = new_doubleArray(4)
    doubleArray_setitem(yObs, 0, 3)
    doubleArray_setitem(yObs, 1, 13)
    doubleArray_setitem(yObs, 2, 1)
    doubleArray_setitem(yObs, 3, 33)

    numObs = 4
    numCoeff = 3

    fit1D = fit(numObs, xObs, yObs, numCoeff);
    print "\n1D Fit from arrays:"
    print fit1D

    delete_doubleArray(xObs)
    delete_doubleArray(yObs)

    #############################
    # 1D Fit test (Vector args) #
    #############################

    xObs = VectorDouble(4)
    xObs[0] = 1
    xObs[1] = -1
    xObs[2] = 2
    xObs[3] = -2

    yObs = VectorDouble(4)
    yObs[0] = 3
    yObs[1] = 13
    yObs[2] = 1
    yObs[3] = 33

    numCoeff = 3

    fit1D = FitVectorDouble(xObs, yObs, numCoeff)
    print "\n1D Fit from Vectors:"
    print fit1D

    ############################
    # 2D Fit test (array args) #
    ############################

    numRows = 3
    numCols = 3

    xObs = new_doubleArray(9)
    doubleArray_setitem(xObs, 0, 1)
    doubleArray_setitem(xObs, 1, 0)
    doubleArray_setitem(xObs, 2, 1)
    doubleArray_setitem(xObs, 3, 1)
    doubleArray_setitem(xObs, 4, 1)
    doubleArray_setitem(xObs, 5, 0)
    doubleArray_setitem(xObs, 6, 0)
    doubleArray_setitem(xObs, 7, 1)
    doubleArray_setitem(xObs, 8, 1)

    yObs = new_doubleArray(9)
    doubleArray_setitem(yObs, 0, 1)
    doubleArray_setitem(yObs, 1, 1)
    doubleArray_setitem(yObs, 2, 1)
    doubleArray_setitem(yObs, 3, 0)
    doubleArray_setitem(yObs, 4, 1)
    doubleArray_setitem(yObs, 5, 1)
    doubleArray_setitem(yObs, 6, 0)
    doubleArray_setitem(yObs, 7, 0)
    doubleArray_setitem(yObs, 8, 1)

    zObs = new_doubleArray(9)
    doubleArray_setitem(zObs, 0, 1)
    doubleArray_setitem(zObs, 1, 0.3)
    doubleArray_setitem(zObs, 2, 0)
    doubleArray_setitem(zObs, 3, 0.16)
    doubleArray_setitem(zObs, 4, 1)
    doubleArray_setitem(zObs, 5, 0)
    doubleArray_setitem(zObs, 6, 0)
    doubleArray_setitem(zObs, 7, 0)
    doubleArray_setitem(zObs, 8, 0.85)

    fit2D = fit(numRows, numCols, xObs, yObs, zObs, 1, 1)
    print "\n2D Fit from arrays:"
    print fit2D

    delete_doubleArray(xObs)
    delete_doubleArray(yObs)
    delete_doubleArray(zObs)

    #############################
    # 2D Fit test (Matrix args) #
    #############################

    xObs = MatrixDouble(3,3)
    xObs[0,0] = 1
    xObs[0,1] = 0
    xObs[0,2] = 1
    xObs[1,0] = 1
    xObs[1,1] = 1
    xObs[1,2] = 0
    xObs[2,0] = 0
    xObs[2,1] = 1
    xObs[2,2] = 1

    yObs = MatrixDouble(3,3)
    yObs[0,0] = 1
    yObs[0,1] = 1
    yObs[0,2] = 1
    yObs[1,0] = 0
    yObs[1,1] = 1
    yObs[1,2] = 1
    yObs[2,0] = 0
    yObs[2,1] = 0
    yObs[2,2] = 1

    zObs = MatrixDouble(3,3)
    zObs[0,0] = 1
    zObs[0,1] = 0.3
    zObs[0,2] = 0
    zObs[1,0] = 0.16
    zObs[1,1] = 1
    zObs[1,2] = 0
    zObs[2,0] = 0
    zObs[2,1] = 0
    zObs[2,2] = 0.85

    fit2D = fit(xObs, yObs, zObs, 1, 1)
    print "2D Fit from Matrices:"
    print fit2D

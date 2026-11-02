#!/usr/bin/env python

"""
 * =========================================================================
 * This file is part of math.poly-python
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * math.poly-python is free software; you can redistribute it and/or modify
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
import numpy as np
from coda.math_linear import VectorDouble, MatrixDouble, Vector3
from coda.math_poly import *

if sys.version_info[0] == 2:
    import cPickle as pickle
else:
    import pickle

if __name__ == '__main__':
    #################
    # Basic 1D test #
    #################

    poly1D = Poly1D(2)
    for x in range(poly1D.order() + 1):
        poly1D[x] = (x + 1) * 10

    print('1D poly:')
    print(poly1D)

    # Try to index out of bounds by getting
    threw = False
    try:
        foo = poly1D[3]
    except ValueError:
        threw = True

    if threw:
        print('Getting 1D OOB threw as expected')
    else:
        sys.exit('Getting 1D OOB did not throw!')

    # Try to index out of bounds by setting
    threw = False
    try:
        poly1D[3] = 5
    except ValueError:
        threw = True

    if threw:
        print('Setting 1D OOB threw as expected')
    else:
        sys.exit('Setting 1D OOB did not throw!')

    # Pickle and unpickle
    pPoly1D = pickle.loads(pickle.dumps(poly1D))
    if pPoly1D.coeffs() == poly1D.coeffs():
        print('Pickling and unpickling 1D matched as expected')
    else:
        sys.exit('Pickling 1D did not match!')

    #################
    # Basic 2D test #
    #################

    poly2D = Poly2D(2, 3)
    val = 100
    for x in range(poly2D.orderX() + 1):
        for y in range(poly2D.orderY() + 1):
            poly2D[(x, y)] = val
            val += 100

    print('\n2D poly:')
    print(poly2D)

    # Try to index out of bounds by getting
    threw = False
    try:
        foo = poly2D[(3, 3)]
    except ValueError:
        threw = True

    if threw:
        print('Getting 2D OOB threw as expected')
    else:
        sys.exit('Getting 2D OOB did not throw!')

    # Try to index out of bounds by setting
    threw = False
    try:
        poly2D[(3, 3)] = 5
    except ValueError:
        threw = True

    if threw:
        print('Setting 2D OOB threw as expected')
    else:
        sys.exit('Setting 2D OOB did not throw!')

    # Pickle and unpickle
    pPoly2D = pickle.loads(pickle.dumps(poly2D))
    if pPoly2D == poly2D:
        print('Pickling and unpickling 2D matched as expected')
    else:
        sys.exit('Pickling 2D did not match!')

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
    print("\n1D Fit from arrays:")
    print(fit1D)

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
    print("\n1D Fit from Vectors:")
    print(fit1D)

    ############################################
    # PolyVector3 Fit Test (math::linear args) #
    ############################################

    xObs = VectorDouble(4)
    xObs[0] = 1
    xObs[1] = -1
    xObs[2] = 2
    xObs[3] = -2

    yObs = MatrixDouble(3,4)
    yObs[0,0] = 3
    yObs[0,1] = 13
    yObs[0,2] = 1
    yObs[0,3] = 33
    yObs[1,0] = 33
    yObs[1,1] = 1
    yObs[1,2] = 13
    yObs[1,3] = 3
    yObs[2,0] = 1
    yObs[2,1] = -1
    yObs[2,2] = 2
    yObs[2,3] = -2

    numCoeff = 3

    fitPolyVector3 = fit(xObs, yObs, numCoeff)
    print("\nPolyVector3 fit from math::linear args:")
    print(fitPolyVector3)

    ###########################################
    # PolyVector3 Fit Test (std::vector args) #
    ###########################################

    xObs = StdVectorDouble([1, -1, 2, -2])
    yObs0 = StdVectorDouble([3, 13, 1, 33])
    yObs1 = StdVectorDouble([33, 1, 13, 3])
    yObs2 = StdVectorDouble([1, -1, 2, -2])
    numCoeff = 3

    fitPolyVector3 = fit(xObs, yObs0, yObs1, yObs2, numCoeff)
    print("\nPolyVector3 fit from std::vector args:")
    print(fitPolyVector3)

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
    print("\n2D Fit from arrays:")
    print(fit2D)

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
    print("2D Fit from Matrices:")
    print(fit2D)

    ##########################################
    # Test polynomial evaluation using lists #
    ##########################################

    input_data = [1.0, 2.0, -3.0, 5.0]

    #--------#
    # Poly1D #
    #--------#
    p1 = Poly1D(2)
    p1[0] =  1.0
    p1[1] =  0.0
    p1[2] = -3.0

    threw = False
    try:
        vals1 = p1(input_data)
    except TypeError:
        threw = True

    if threw:
        print("Poly1D error: polynomial evaluation using a Python list failed.")
    else:
        for x,val in zip(input_data, vals1):
            if val != p1(x):
                raise ValueError("Poly1D error: polynomial evaluation with " +
                                 "Python list does not return the same " +
                                 "value as scalar evaluation.")
        print("\nPoly1D evaluation using a Python list passed:")
        print(p1)
        print("input  : ", input_data)
        print("output : ", vals1)


    #--------#
    # Poly2D #
    #--------#
    p2 = Poly2D(1,1)
    p2[0,0] =  5.0
    p2[0,1] =  0.0
    p2[1,0] = -1.0
    p2[1,1] =  1.0

    threw = False
    try:
        vals2 = p2(input_data, input_data)
    except TypeError:
        threw = True

    if threw:
        print("Poly2D error: polynomial evaluation using Python lists failed.")
    else:
        for x,val in zip(input_data, vals2):
            if val != p2(x,x):
                raise ValueError("Poly2D error: polynomial evaluation with " +
                                 "Python list does not return the same " +
                                 "value as scalar evaluation.")
        print("\nPoly2D evaluation using Python lists passed:")
        print(p2)
        print("input  : ", input_data, ",", input_data)
        print("output : ", vals2)

    #-------------#
    # PolyVector3 #
    #-------------#
    p3 = PolyVector3(2)
    p3[0] = Vector3([ 1.0,  0.0, 3.0])
    p3[1] = Vector3([-1.0,  2.0, 0.0])
    p3[2] = Vector3([ 0.0, -5.0, 0.0])

    threw = False
    try:
        vals3 = p3(input_data)
    except TypeError:
        threw = True

    if threw:
        print("PolyVector3 error: polynomial evaluation using a Python list failed.")
    else:
        for x,val in zip(input_data, vals3):
            if not all([a == b for a,b in zip(val.vals(), p3(x).vals())]):
                raise ValueError("PolyVector3 error: polynomial evaluation "+
                                 "with Python list does not return the same " +
                                 "value as scalar evaluation.")
        print("\nPolyVector3 evaluation using a Python list passed:")
        print(p3)
        print("input  : ", input_data)
        print("output : ", [p.vals() for p in vals3])


    #############################
    # 1D Numpy converstion test #
    #############################
    original = Poly1D([1, 2, 3])
    converted = Poly1D.fromArray(original.asArray())
    print("Converting 1D polynomial to numpy array")
    if original == converted:
        print("Converstion successful")
    else:
        raise ValueError("Converstion to numpy array failed")
    assert isinstance(original.asArray(), np.ndarray)


    ############################
    # 2D Numpy conversion test #
    ############################
    original = Poly2D(1, 2)
    original[0,0] =  5.0
    original[0,1] =  0.0
    original[0,2] =  2.6
    original[1,0] = -1.0
    original[1,1] =  1.0
    original[1,2] = -8.4
    converted = Poly2D.fromArray(original.asArray())
    print("Converting 2D polynomial to numpy array")
    if original == converted:
        print("Converstion successful")
    else:
        raise ValueError("Converstion to numpy array failed")
    assert isinstance(original.asArray(), np.ndarray)



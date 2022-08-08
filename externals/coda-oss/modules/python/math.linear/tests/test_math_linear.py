#!/usr/bin/env python

"""
 * =========================================================================
 * This file is part of math.linear-c++
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
import numpy as np
from copy import deepcopy
from coda.math_linear import *

if sys.version_info[0] == 2:
    import cPickle as pickle
else:
    import pickle

if __name__ == '__main__':

    ####################################
    # VectorN<2, double> Bindings Test #
    ####################################

    vec2 = Vector2()
    vec2[0] = 100
    vec2[1] = 200
    print("Vector2:")
    print(vec2)

    # Test deep copy
    vec2_copy = deepcopy(vec2)
    if vec2_copy[0] != 100 or vec2_copy[1] != 200:
        sys.exit('Vector2 did not perform a deep copy')

    vec2_copy[0] = 300
    if vec2[0] != 100:
        sys.exit('Vector2 did not perform a deep copy')

    # Test pickling and unpickling
    pvec2 = pickle.loads(pickle.dumps(vec2))
    for i in range(2):
        if pvec2[i] != vec2[i]:
            sys.exit('Vector2 was not pickled correctly')

    ####################################
    # VectorN<3, double> Bindings Test #
    ####################################

    vec3 = Vector3()
    vec3[0] = 10
    vec3[1] = 20
    vec3[2] = 30
    print("Vector3:")
    print(vec3)

    vec3_copy = deepcopy(vec3)
    if vec3_copy[0] != 10 or vec3_copy[1] != 20 or vec3_copy[2] != 30:
        sys.exit('Vector3 did not perform a deep copy')

    vec3_copy[0] = 40
    if vec3[0] != 10:
        sys.exit('Vector3 did not perform a deep copy')

    # Test pickling and unpickling
    pvec3 = pickle.loads(pickle.dumps(vec3))
    for i in range(3):
        if pvec3[i] != vec3[i]:
            sys.exit('Vector3 was not pickled correctly')

    ################################
    # Vector<double> Bindings Test #
    ################################

    size = 3
    vecD = VectorDouble(size)
    vecD[0] = 10
    vecD[1] = 20
    vecD[2] = 30
    print("\nVectorDouble:")
    print(vecD)

    # NumPy compatibility test
    example = np.asarray([10,20,30])
    converted = np.asarray(vecD.vals())
    print("np.array_equal(example, converted):",np.array_equal(example, converted))

    # Try to get the -1st element
    threw = False
    try:
        foo = vecD[-1]
        print(foo)
    except ValueError:
        threw = True

    if threw:
        print('\nGetting (OOB low) threw as expected')
    else:
        sys.exit('Getting (OOB low) did not throw!')

    # Try to get the N+1th element
    threw = False
    try:
        foo = vecD[size]
        print(foo)
    except ValueError:
        threw = True

    if threw:
        print('Getting (OOB high) threw as expected')
    else:
        sys.exit('Getting (OOB high) did not throw!')

    # Try to set the -1st element
    threw = False
    try:
        vecD[-1] = 100
        print(foo)
    except ValueError:
        threw = True

    if threw:
        print('Setting (OOB low) threw as expected')
    else:
        sys.exit('Setting (OOB low) did not throw!')

    # Try to set the N+1th element
    threw = False
    try:
        vecD[size] = 100
        print(foo)
    except ValueError:
        threw = True

    if threw:
        print('Setting (OOB high) threw as expected')
    else:
        sys.exit('Setting (OOB high) did not throw!')

    ##################################
    # Matrix2D<double> Bindings Test #
    ##################################

    dim = 3
    matrixD = MatrixDouble(dim, dim)
    matrixD[0,0] = 0
    matrixD[0,1] = 1
    matrixD[0,2] = 2
    matrixD[1,0] = 3
    matrixD[1,1] = 4
    matrixD[1,2] = 5
    matrixD[2,0] = 6
    matrixD[2,1] = 7
    matrixD[2,2] = 8
    print("\nMatrix2D:")
    print(matrixD)

    # NumPy compatibility test
    example = np.asarray([[0,1,2],[3,4,5],[6,7,8]])
    converted = np.asarray(matrixD.vals())
    print(("np.array_equal(example, converted):",np.array_equal(example, converted)))

    # Try to get a value in the -1st row
    threw = False
    try:
        foo = matrixD[-1,0]
    except ValueError:
        threw = True

    if threw:
        print('\nGetting (OOB low row) threw as expected')
    else:
        sys.exit('Getting (OOB low row) did not throw!')

    # Try to get a value in the N+1th row
    threw = False
    try:
        foo = matrixD[dim,0]
    except ValueError:
        threw = True

    if threw:
        print('Getting (OOB high row) threw as expected')
    else:
        sys.exit('Getting (OOB high row) did not throw!')

    # Try to get a value in the -1st column
    threw = False
    try:
        foo = matrixD[0,-1]
    except ValueError:
        threw = True

    if threw:
        print('Getting (OOB low column) threw as expected')
    else:
        sys.exit('Getting (OOB low column) did not throw!')

    # Try to get a value in the N+1th column
    threw = False
    try:
        foo = matrixD[0,dim]
    except ValueError:
        threw = True

    if threw:
        print('Getting (OOB high column) threw as expected')
    else:
        sys.exit('Getting (OOB high column) did not throw!')

    # Try to get a value in a non-integer row
    threw = False
    try:
        foo = matrixD[0.5,0]
    except TypeError:
        threw = True

    if threw:
        print('Getting (Non-integer row) threw as expected')
    else:
        sys.exit('Getting (Non-integer row) did not throw!')

    # Try to get a value in a non-integer column
    threw = False
    try:
        foo = matrixD[0,0.5]
    except TypeError:
        threw = True

    if threw:
        print('Getting (Non-integer column) threw as expected')
    else:
        sys.exit('Getting (Non-integer column) did not throw!')

    # Try to get a value with too few arguments
    threw = False
    try:
        foo = matrixD[0]
    except TypeError:
        threw = True

    if threw:
        print('Getting (Too few arguments) threw as expected')
    else:
        sys.exit('Getting (Too few arguments) did not throw!')

    # Try to get a value with too many arguments
    threw = False
    try:
        foo = matrixD[0,1,2]
    except TypeError:
        threw = True

    if threw:
        print('Getting (Too many arguments) threw as expected')
    else:
        sys.exit('Getting (Too many arguments) did not throw!')

    # Try to set a value in the -1st row
    threw = False
    try:
        matrixD[-1,0] = 100
    except ValueError:
        threw = True

    if threw:
        print('Setting (OOB low row) threw as expected')
    else:
        sys.exit('Setting (OOB low row) did not throw!')

    # Try to set a value in the N+1th row
    threw = False
    try:
        matrixD[dim,0] = 100
    except ValueError:
        threw = True

    if threw:
        print('Setting (OOB high row) threw as expected')
    else:
        sys.exit('Setting (OOB high row) did not throw!')

    # Try to set a value in the -1st column
    threw = False
    try:
        matrixD[0,-1] = 100
    except ValueError:
        threw = True

    if threw:
        print('Setting (OOB low column) threw as expected')
    else:
        sys.exit('Setting (OOB low column) did not throw!')

    # Try to set a value in the N+1th column
    threw = False
    try:
        matrixD[0,dim] = 100
    except ValueError:
        threw = True

    if threw:
        print('Setting (OOB high column) threw as expected')
    else:
        sys.exit('Setting (OOB high column) did not throw!')

    # Try to set a value in a non-integer row
    threw = False
    try:
        matrixD[0.5,0] = 100
    except TypeError:
        threw = True

    if threw:
        print('Setting (Non-integer row) threw as expected')
    else:
        sys.exit('Setting (Non-integer row) did not throw!')

    # Try to set a value in a non-integer column
    threw = False
    try:
        matrixD[0,0.5] = 100
    except TypeError:
        threw = True

    if threw:
        print('Setting (Non-integer column) threw as expected')
    else:
        sys.exit('Setting (Non-integer column) did not throw!')

    # Try to set a value with too few arguments
    threw = False
    try:
        matrixD[0] = 100
    except TypeError:
        threw = True

    if threw:
        print('Setting (Too few arguments) threw as expected')
    else:
        sys.exit('Setting (Too few arguments) did not throw!')

    # Try to set a value with too many arguments
    threw = False
    try:
        matrixD[0,1,2] = 100
    except TypeError:
        threw = True

    if threw:
        print('Setting (Too many arguments) threw as expected')
    else:
        sys.exit('Setting (Too many arguments) did not throw!')

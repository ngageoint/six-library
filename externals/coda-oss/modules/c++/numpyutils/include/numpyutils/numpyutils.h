/* =========================================================================
 * This file is part of CODA-OSS
 * =========================================================================
 *
 * C) Copyright 2004 - 2016, MDA Information Systems LLC
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
 */

#ifndef __NUMPYUTILS_NUMPYUTILS_H__
#define __NUMPYUTILS_NUMPYUTILS_H__

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <Python.h>
#include <numpy/arrayobject.h>
#include <types/RowCol.h>
#include <vector>

namespace numpyutils
{

/*! Verifies that the input object is a PyArray object
 * \param pyObject object to test
 * \throws except::Exception if pyObject isn't an instance of PyArray
 */
void verifyArray(PyObject *pyObject);

/* Verifies that the input object element type matches the input typeNum)
 * \param pyObject Instance of PyArrayObject to test
 * \param typeNum python type number of desired type (see python API docs)
 * \throws except::Exception if pyObject's typeNum != typeNum
 */
void verifyType(PyObject* pyObject, int typeNum);

/*! Verifies both that the input object is a PyArray and that its element type
 *      matches the input typeNume
 * \param pyObject object to test
 * \param typeNum desired python type num
 * \throws except::Exception if pyObject isn't a PyArray or if types do not match
 */
void verifyArrayType(PyObject *pyObject, int typeNum);

/*! Returns array dimensions and enforces a dimension check of two
 * \param pyArrayObject array object to inspect
 * \returns read only pointer to the dimensions of array
 * \throws except::Exception if not a 2D array
 */
const npy_intp* const getDimensions(PyObject* pyArrayObject);

/*! Variant returning types::RowCol<size_t> version of dimensions
 * \param pyArrayObject array object to inspect
 * \returns row col of dimensions
 * \throws except::Exception if not a 2D array
 */
types::RowCol<size_t> getDimensionsRC(PyObject* pyArrayObject);

/*! Verifies that the objects are of the same dimensions
 * \param lhs comparison object
 * \param rhs comparison object
 * \throws except::Exception if objects dimensions don't match
 * */
void verifyObjectsAreOfSameDimensions(PyObject* lhs, PyObject* rhs);

/*!
 * Helper function used to either verify that an object is either
 * an array with the requested dimensions and type OR create a
 * new array of the requested dimensions and type, if not.
 * \param pyObject None or array to verify
 * \param typeNum desired type number
 * \param dims desired dimensions of array
 * \throws except::Exception if pyObject is not None and doesn't match
 *              specified parameters
 */
void createOrVerify(PyObject*& pyObject, int typeNum, const types::RowCol<size_t>& dims);

/*!
 * Verifies Array Type and TypeNum for input and output.  If output
 * array is Py_None, constructs a new PyArray of the desired specifications
 * \param pyInObject array to verify
 * \param pyOutObject either None or array to verify
 * \param inputTypeNum input type
 * \param outputTypeNum output type
 * \param dims Desired output dimensions.
 * \throws except::Exception if input/output types don't match,
 *                           if output isn't None and doesn't match input
 *                                type and dims
 */
void prepareInputAndOutputArray(PyObject* pyInObject,
                                PyObject*& pyOutObject,
                                int inputTypeNum,
                                int outputTypeNum,
                                types::RowCol<size_t> dims);

/*!
 * Verifies Array Type and TypeNum for input and output.  If output
 * array is Py_None, constructs a new PyArray of the desired specifications.
 * Uses the dimensions of the input object for the output object.
 * \param pyInObject array to verify
 * \param pyOutObject either None or array to verify
 * \param inputTypeNum input type
 * \param outputTypeNum output type
 * \throws except::Exception if input type doesn't match,
 *                           if output isn't None and doesn't match input
 *                                type and dims
 */
void prepareInputAndOutputArray(PyObject* pyInObject,
                                PyObject*& pyOutObject,
                                int inputTypeNum,
                                int outputTypeNum);

/*!
 * Copy a data buffer into a 1/2D Numpy array
 * \param numRows number of rows. Pass 1 for a 1D array
 * \param numColumns number of columns
 * \param typenum value of desired datatype
 * \param data the data to be wrapped
 * \return a numpy array with specified dimension and type
 */
PyObject* toNumpyArray(size_t numRows, size_t numColumns, int typenum,
        void* data);

/*!
 * Copy a vector of data buffers into a 2D Numpy array
 * \param numColumns number of columns
 * \param typenum value of desired datatype
 * \param data the data to be wrapped
 * \return a numpy array with specified dimension and type
 */

PyObject* toNumpyArray(size_t numColumns, int typenum,
        const std::vector<void*>& data);

/*!
 * Helper function to get data array.
 * \param pyInObject array to get data buffer of
 * \returns a pointer to the array's buffer interpreted as an array of type char*
 * */
char* getDataBuffer(PyArrayObject* pyInObject);

/*! Extract PyArray Buffer as raw array of type T*
 * \param pyObject object to turn into raw pointer
 * \returns a pointer to the array's buffer interpreted as an array of type T
 */
template<typename T>
T* getBuffer(PyObject* pyObject)
{
    return reinterpret_cast<T*>(
            getDataBuffer(
                reinterpret_cast<PyArrayObject*>(pyObject)));
}

}

#endif

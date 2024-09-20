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

#pragma once
#ifndef __NUMPYUTILS_NUMPYUTILS_H__
#define __NUMPYUTILS_NUMPYUTILS_H__

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include <config/compiler_extensions.h>
CODA_OSS_disable_warning_system_header_push
#if _MSC_VER
#pragma warning(disable: 26493) // Don't use C-style casts (type.4).
#pragma warning(disable: 26494) // Variable '...' is uninitialized. Always initialize an object (type.5).
#pragma warning(disable: 26451) // Arithmetic overflow: Using operator '...' on a 4 byte value and then casting the result to a 8 byte value. Cast the value to the wider type before calling operator '...' to avoid overflow (io.2).
#pragma warning(disable: 26814) // The const variable '...' can be computed at compile-time. Consider using constexpr (con.5).
#pragma warning(disable: 26447) // The function is declared '...' but calls function '...' which may throw exceptions (f.6).
#pragma warning(disable: 26455) // Default constructor should not throw. Declare it '...' (f.6).
#pragma warning(disable: 26440)  // Function '...' can be declared '...' (f.6).
#pragma warning(disable: 26496) // The variable '...' does not change after construction, mark it as const (con.4).
#pragma warning(disable: 26462) // The value pointed to by '...' is assigned only once, mark it as a pointer to const (con.4).
#pragma warning(disable: 26435) // Function '...' should specify exactly one of '...', '...', or '...' (c.128).
#pragma warning(disable: 26433) // Function '...' should be marked with '...' (c.128).
#pragma warning(disable: 26497) // You can attempt to make '...' constexpr unless it contains any undefined behavior (f.4).
#pragma warning(disable: 26433) // Function '...' should be marked with '...' (c.128).
#pragma warning(disable: 26456) // Operator '...' hides a non-virtual operator '...' (c.128).
#pragma warning(disable: 26446) // Prefer to use gsl::at() instead of unchecked subscript operator (bounds.4).
#pragma warning(disable: 26477)  // Use '...' rather than 0 or NULL(es .47).
#pragma warning(disable: 26457) // (void) should not be used to ignore return values, use '...' instead (es.48).
#pragma warning(disable: 26813) // Use '...' to check if a flag is set.
#else
CODA_OSS_disable_warning(-Wshadow)
CODA_OSS_disable_warning(-Wsuggest-override)
CODA_OSS_disable_warning(-Wzero-as-null-pointer-constant)
#endif

#include <Python.h>
#include <numpy/arrayobject.h>
CODA_OSS_disable_warning_pop

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
const npy_intp* getDimensions(PyObject* pyArrayObject);

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
        const void* data);

/*
 * Get the number of elements in a numpy array.
 * \param pyArrayObject Numpy array to count
 * \return Number of elements in pyArrayObject
 * \throws if pyArrayObject is not a PyArrayObject
 */
size_t getNumElements(PyObject* pyArrayObject);


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

/*!
 * Call this function after creating new PyObject with Numpy C API
 * If a problem occurred, print error message and throw exception
 * \param object Array to check
 */
void verifyNewPyObject(PyObject* object);

}

#endif

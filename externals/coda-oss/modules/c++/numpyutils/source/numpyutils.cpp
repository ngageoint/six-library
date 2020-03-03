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

#include <numpyutils/numpyutils.h>
#include <except/Exception.h>
#include <sys/Conf.h>

/*
 * import_array is actually a macro and will return differently depending on
 * the python version.  See
 * https://mail.scipy.org/pipermail/numpy-discussion/2010-December/054350.html
 * for the source and some discussion
 */
#if PY_MAJOR_VERSION >= 3
void* init_numpy()
{
    import_array();
    return nullptr;
}
#else
void init_numpy()
{
    import_array();
}
#endif

/* Numpy uses some static variables which are per-compilation
 * unit and if import_arrays is not called, will segfault on using any of
 * the numpy functions.  This is a run-time issue, not a compile time issue
 * so it can be painful to detect.
 *
 * The basic mechanism used here should also be followed in other numpyutil
 * source files.  The idea is to use a static declaration to force a constructor
 * call which in turn calls import_array() and sets up the requisite run time
 * variables.
 *
 * This implementation has been tested on Linux/Windows, but in case it
 * breaks down in the future for any reason, there are some alternatives:
 *
 * (1) Go to header-only and rely on import_array() in any dependencies
 * [all c++ files and any .i files]
 *
 * (2) Instead of importing in every compilation unit we could just do in
 * numpyutils.cpp  * with PY_ARRAY_UNIQUE_SYMBOL defined and add that #define
 * NO_IMPORT_ARRAY everywhere else.
 *
 * See: http://docs.scipy.org/doc/numpy/reference/c-api.array.html#importing-the-api
 *
 * This could manifest as something like
 *
 * #define NO_IMPORT_ARRAY
 * #define PY_ARRAY_UNIQUE_SYMBOL CODA_ARRAY_API
 * in numpyutils.h and an import_array somewhere once.
 *
 */

struct InitializeNumPy
{
    InitializeNumPy()
    {
        Py_Initialize();
        init_numpy();
    }
};

static InitializeNumPy npyinit;

namespace numpyutils
{

void verifyArray(PyObject *pyObject)
{
    if (!PyArray_Check(pyObject))
    {
        throw except::Exception(Ctxt(
                    "Invalid data type (expected numpy array)"));
    }
}

void verifyType(PyObject* pyObject, int typeNum)
{
    if (PyArray_TYPE(reinterpret_cast<PyArrayObject*>(pyObject)) != typeNum)
    {
        throw except::Exception(Ctxt("Unexpected numpy array element type"));
    }
}

void verifyArrayType(PyObject *pyObject, int typeNum)
{
    verifyArray(pyObject);
    verifyType(pyObject, typeNum);
}

const npy_intp* const getDimensions(PyObject* pyArrayObject)
{
    verifyArray(pyArrayObject);
    int ndims = PyArray_NDIM(reinterpret_cast<PyArrayObject*>(pyArrayObject));
    if (ndims != 2)
    {
        throw except::Exception(Ctxt(
                    "Numpy array has dimensions different than 2"));
        return 0;
    }
    return PyArray_DIMS(reinterpret_cast<PyArrayObject*>(pyArrayObject));
}

types::RowCol<size_t> getDimensionsRC(PyObject* pyArrayObject)
{
   const npy_intp* dims = getDimensions(pyArrayObject);
   return types::RowCol<size_t>(dims[0], dims[1]);
}

void verifyObjectsAreOfSameDimensions(PyObject* lhs,
                                      PyObject* rhs)
{
    if(getDimensionsRC(lhs) != getDimensionsRC(rhs))
    {
        throw except::Exception(Ctxt(
                    "Numpy arrays are of differing dimensions"));
    }
}

void createOrVerify(PyObject*& pyObject,
                    int typeNum,
                    const types::RowCol<size_t>& dims)
{
    if (pyObject == Py_None) // none passed in-- so create new
    {
        npy_intp odims[2] = {static_cast<npy_intp>(dims.row), static_cast<npy_intp>(dims.col)};
        pyObject = PyArray_SimpleNew(2, odims, typeNum);
        verifyNewPyObject(pyObject);
    }
    else
    {
        verifyArrayType(pyObject, typeNum);
        const npy_intp* const outdims = getDimensions(pyObject);
        if (outdims[0] != static_cast<npy_intp>(dims.row)  || outdims[1] != static_cast<npy_intp>(dims.col))
        {
            throw except::Exception(Ctxt(
                        "Desired array does not match required row, cols"));
        }
    }
}

PyObject* toNumpyArray(size_t numRows, size_t numColumns,
        int typenum, const void* data)
{
    const int nDims = (numRows == 1 ? 1 : 2);
    npy_intp dimensions[2];
    if (nDims == 1)
    {
        dimensions[0] = numColumns;
    }
    else
    {
        dimensions[0] = numRows;
        dimensions[1] = numColumns;
    }

    // PyArray_SimpleNewFromData takes a void* for the buffer it's wrapping.
    // The buffer is non-const, and this makes sense because we're wrapping it
    // in a numpy array which anyone else would be free to modify.
    //
    // Since the function shouldn't be modifying the data itself, we're casting
    // away the const in order to get the intermediate array object.  We then
    // immediately copy and return, so nothing is actually getting modified
    PyObject* wrappedArray =
            PyArray_SimpleNewFromData(nDims, dimensions, typenum,
                                      const_cast<void*>(data));
    PyObject* copy = PyArray_NewCopy(reinterpret_cast<PyArrayObject*>(wrappedArray),
            NPY_CORDER);
    verifyNewPyObject(copy);
    return copy;
}

PyObject* toNumpyArray(size_t numColumns, int typenum,
        const std::vector<void*>& data)
{
    const size_t numRows = data.size();
    PyObject* list = PyList_New(numRows);
    verifyNewPyObject(list);
    for (size_t ii = 0; ii < numRows; ++ii)
    {
        npy_intp dimensions[] = { numColumns };
        PyObject* row =
                PyArray_SimpleNewFromData(1, dimensions, typenum, data[ii]);
        verifyNewPyObject(row);
        PyList_SET_ITEM(list, ii, row);
    }
    PyObject* newArray = PyArray_FROM_O(list);
    verifyNewPyObject(newArray);
    return newArray;
}

size_t getNumElements(PyObject* pyArrayObject)
{
    verifyArray(pyArrayObject);
    return PyArray_Size(pyArrayObject);
}

void prepareInputAndOutputArray(PyObject* pyInObject,
                                PyObject*& pyOutObject,
                                int inputTypeNum,
                                int outputTypeNum,
                                types::RowCol<size_t> dims)
{
    verifyArrayType(pyInObject, inputTypeNum);
    createOrVerify(pyOutObject, outputTypeNum, dims);
}

void prepareInputAndOutputArray(PyObject* pyInObject,
                                PyObject*& pyOutObject,
                                int inputTypeNum,
                                int outputTypeNum)
{
    prepareInputAndOutputArray(pyInObject,
                               pyOutObject,
                               inputTypeNum,
                               outputTypeNum,
                               getDimensionsRC(pyInObject));
}

char* getDataBuffer(PyArrayObject* pyInObject)
{
   return PyArray_BYTES(pyInObject);
}

void verifyNewPyObject(PyObject* object)
{
    if (!object)
    {
        PyErr_Print();
        throw except::Exception(Ctxt("An error occurred while trying "
                    "to create a PyObject"));
    }
}

}

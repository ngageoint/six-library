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

%module(package="coda") numpyutilstest

%feature("autodoc","1");


%include <std_vector.i>
%include <std_string.i>

%{
#include <memory>
#include <numpyutils/numpyutils.h>
#include <except/Exception.h>
%}

%import <types.i>
%import <except.i>

%inline
%{
    PyObject* elementDoubleTest(PyObject* inputNPArray, PyObject* outputNPArray = Py_None)
    {
        const types::RowCol<size_t> dims = numpyutils::getDimensionsRC(inputNPArray);
        numpyutils::prepareInputAndOutputArray(inputNPArray, outputNPArray,NPY_FLOAT32, NPY_FLOAT32, dims);

        const float* input = numpyutils::getBuffer<float>(inputNPArray);
        float* output = numpyutils::getBuffer<float >(outputNPArray);

        for(size_t i=0;i<dims.area();i++)
        {
            output[i] = input[i] * 2;
        }

        return Py_BuildValue("O", outputNPArray);
    }
%}

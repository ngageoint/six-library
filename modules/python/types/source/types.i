/*
 * =========================================================================
 * This file is part of coda_types-python
 * =========================================================================
 *
 * (C) Copyright 2004 - 2015, MDA Information Systems LLC
 *
 * coda_types-python is free software; you can redistribute it and/or modify
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
 */

%module(package="coda") coda_types

%feature("autodoc", "1");

%include "config.i"

%include <std_vector.i>
%include <std_string.i>

%import "sys.i"

%{
  #include "sys/Conf.h"
  #include "types/RowCol.h"
  #include "types/RgAz.h"
%}

%include "types/RowCol.h"
%include "types/RgAz.h"

// Pickle utilities
%pythoncode
%{
    from . import pickle
%}

%extend types::RowCol
{
%pythoncode
%{
    def __getstate__(self):
        return (self.row, self.col)

    def __setstate__(self, state):
        self.__init__(state[0], state[1])
%}
}

%extend types::RgAz
{
%pythoncode
%{
    def __getstate__(self):
        return (self.rg, self.az)

    def __setstate__(self, state):
        self.__init__(state[0], state[1])
%}
}

%extend std::vector
{
%pythoncode
%{
    def __getstate__(self):
        # Return a nonempty (thus non-false) tuple with dummy value in first position
        return (-1, tuple(pickle.dumps(elem) for elem in self))

    def __setstate__(self, state):
        self.__init__()
        # State will have a dummy entry in the first position
        for elem in state[1]:
            self.push_back(pickle.loads(elem))
%}
}

%template(RowColDouble) types::RowCol<double>;
%template(RowColInt) types::RowCol<sys::SSize_T>;
%template(RowColSizeT) types::RowCol<size_t>;
%template(RgAzDouble) types::RgAz<double>;

%template(VectorRowColInt) std::vector<types::RowCol<sys::SSize_T> >;
%template(VectorRowColDouble) std::vector<types::RowCol<double> >;
%template(VectorSizeT) std::vector<size_t>;
%template(VectorString) std::vector<std::string>;

/* This defines a macro for pickling other generic swig types that do not
 * require special get/set/init options. */

%define EXTEND_GETSTATE(classname)
%extend classname
{
%pythoncode
%{
def __getstate__(self):
    """Recursive pickling method for generic SWIG-wrapped objects."""
    # Create a dictionary of parameters and values
    state = {}

    # Use swig_setmethods to get only data we can set later
    for a in self.__swig_setmethods__:
        b = getattr(self, a)

        # Skip unpicklable data
        if type(b).__name__ == 'SwigPyObject':
            print 'Ignoring unknown SwigPyObject:', b
            continue
        else:
            state[a] = pickle.dumps(b)
    return state
%}
}
%enddef

%define EXTEND_SETSTATE(classname)
%extend classname
{
%pythoncode
%{
def __setstate__(self, state):
    """Recursive unpickling method for generic SWIG-wrapped objects."""
    self.__init__()
    for a,b in state.iteritems():
        setattr(self, a, pickle.loads(b))
%}
}
%enddef

/* This macro will replace the accessor and setter for a fixed-length 2d array member
 * with a copy to/from a numpy array (1d arrays can be handled with szrow = 1) */
%define FIXEDARRAY(classname,arrname,szrow,szcol,datatype,NPYtypecode)
%extend classname
{
    // Make a copy of the data as a numpy array
    PyObject* copy ## arrname()
    {
        types::RowCol<size_t> dims(szrow, szcol);
        PyObject *outputNPArray = Py_None;
        numpyutils::createOrVerify(outputNPArray, NPYtypecode, dims);
        datatype * output = numpyutils::getBuffer<datatype>(outputNPArray);
        memcpy(output, self->arrname, szrow * szcol * sizeof(datatype));
        return Py_BuildValue("O", outputNPArray);
    }
    // Copy the data from a numpy array into the object
    void set ## arrname(PyObject* newData)
    {
        numpyutils::verifyArrayType(newData, NPYtypecode);
        const types::RowCol<size_t> dims = numpyutils::getDimensionsRC(newData);
        if (dims.row != szrow || dims.col != szcol)
        {
            throw except::Exception(Ctxt("Array dimensions do not match."));
        }
        else if (dims.row * dims.col != 0)
        {
            if( self->arrname == NULL )
            {
                throw except::Exception(Ctxt("Data not initialized."));
            }

            datatype * input = numpyutils::getBuffer<datatype>(newData);
            memcpy(self->arrname, input, szrow * szcol * sizeof(datatype));
        }
    }
    %pythoncode
    %{
    __swig_getmethods__['arrname'] = copy ## arrname
    __swig_setmethods__['arrname'] = set ## arrname
    if _newclass: arrname = property(copy ## arrname, set ## arrname)
    %}
}
%enddef

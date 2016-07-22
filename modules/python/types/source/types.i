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

%include "std_vector.i"
%include "std_string.i"

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
    import cPickle as pickle
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
%template(VectorSizeT) std::vector<size_t>;

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

%module coda_types

%feature("autodoc", "1");

%include "std_vector.i"
%include "std_string.i"

%{
  #include "types/RowCol.h"
  #include "types/RgAz.h"
%} 

%include "types/RowCol.h"
%include "types/RgAz.h"
%template(RowColDouble) types::RowCol<double>;
%template(RowColSizeT) types::RowCol<size_t>;
%template(RgAzDouble) types::RgAz<double>;

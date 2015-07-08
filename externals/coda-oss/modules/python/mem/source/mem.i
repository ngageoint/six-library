/*
 * =========================================================================
 * This file is part of mem-python 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2015, MDA Information Systems LLC
 *
 * mem-python is free software; you can redistribute it and/or modify
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
 */

/*
 * This module is only here for C++ API compatibility reasons.
 *
 *
 */

%module(package="coda") mem

%feature("autodoc", "1");

%{
#include "import/mem.h"
using namespace mem;
%}

%include "mem/ScopedCopyablePtr.h"
%include "mem/ScopedCloneablePtr.h"


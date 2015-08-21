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

/*
 * When you have a mem::ScopedCopyablePtr of a particular type, use this
 * macro to have Swig generate bindings for this as well as a function to
 * make it.  For example, if you have
 * mem::ScopedCopyablePtr<myNamespace::MyType>
 * Swig will generate a ScopedCopyableMyType object and a
 * makeCopyableMyType() function to make it.  Note that you need to do
 * foo = makeScopedCopyableMyType()
 * rather than
 * foo = ScopedCopyableMyType()
 * to get proper behavior.
 * Use like this in your Swig interface file:
 * SCOPED_COPYABLE(myNamespace, MyType)
 */
%define SCOPED_COPYABLE(namespace, type)
%template(ScopedCopyable##type) mem::ScopedCopyablePtr<namespace##::##type>;
%{
mem::ScopedCopyablePtr< namespace##::##type > makeScopedCopyable##type()
{
    return mem::ScopedCopyablePtr< namespace##::##type >(new namespace##::##type ());
} 
%}

mem::ScopedCopyablePtr< namespace##::##type > makeScopedCopyable##type();
%enddef

/*
 * Same as above but for ScopedCloneablePtr
 */
%define SCOPED_CLONEABLE(namespace, type)
%template(ScopedCloneable##type) mem::ScopedCloneablePtr<namespace##::##type>;
%{
mem::ScopedCloneablePtr< namespace##::##type > makeScopedCloneable##type()
{
    return mem::ScopedCloneablePtr< namespace##::##type >(new namespace##::##type ());
} 
%}

mem::ScopedCloneablePtr< namespace##::##type > makeScopedCloneable##type();
%enddef

/*
 * Permits renaming the python type, copyable
 */
%define SCOPED_COPYABLE_RENAME(namespace, CppType, PyType)
%template(ScopedCopyable##PyType) mem::ScopedCopyablePtr<namespace##::##CppType>;
%{
mem::ScopedCopyablePtr< namespace##::##CppType > makeScopedCopyable##PyType()
{
    return mem::ScopedCopyablePtr< namespace##::##CppType >(new namespace##::##CppType ());
} 
%}

mem::ScopedCopyablePtr< namespace##::##CppType > makeScopedCopyable##PyType();
%enddef

/*
 * Permits renaming the python type, cloneable
 */
%define SCOPED_CLONEABLE_RENAME(namespace, CppType, PyType)
%template(ScopedCloneable##PyType) mem::ScopedCloneablePtr<namespace##::##CppType>;
%{
mem::ScopedCloneablePtr< namespace##::##CppType > makeScopedCloneable##PyType()
{
    return mem::ScopedCloneablePtr< namespace##::##CppType >(new namespace##::##CppType ());
} 
%}

mem::ScopedCloneablePtr< namespace##::##CppType > makeScopedClonable##PyType();
%enddef

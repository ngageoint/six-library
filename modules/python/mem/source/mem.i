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

%include "config.i"

%include <std_auto_ptr.i>

%{
#include "import/mem.h"
using namespace mem;
%}

%include "mem/ScopedCopyablePtr.h"
%include "mem/ScopedCloneablePtr.h"
%include "mem/SharedPtr.h"

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

/*
 * Permits renaming the python type, shared pointer
 */
%define SHARED_RENAME(namespace, CppType, PyType)
%ignore mem::SharedPtr< namespace##::##CppType >::SharedPtr(std::auto_ptr< namespace##::##CppType >);
%template(StdAuto##PyType) std::auto_ptr< namespace##::##CppType >;
%template(Shared##PyType) mem::SharedPtr<namespace##::##CppType>;
%{
mem::SharedPtr< namespace##::##CppType > makeShared##PyType()
{
    return mem::SharedPtr< namespace##::##CppType >(new namespace##::##CppType ());
}
%}

mem::SharedPtr< namespace##::##CppType > makeShared##PyType();
%enddef

/*
 * Permits renaming the python type, copyable
 */
%define SCOPED_COPYABLE_RENAME(namespace, CppType, PyType)
%ignore mem::ScopedCopyablePtr< namespace##::##CppType >::ScopedCopyablePtr(std::auto_ptr< namespace##::##CppType >);
%ignore mem::ScopedCopyablePtr< namespace##::##CppType >::reset(std::auto_ptr< namespace##::##CppType >);
%template(StdAuto##PyType) std::auto_ptr< namespace##::##CppType >;
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
%ignore mem::ScopedCloneablePtr< namespace##::##CppType >::ScopedCloneablePtr(std::auto_ptr< namespace##::##CppType >);
%ignore mem::ScopedCloneablePtr< namespace##::##CppType >::reset(std::auto_ptr< namespace##::##CppType >);
%template(StdAuto##PyType) std::auto_ptr< namespace##::##CppType >;
%template(ScopedCloneable##PyType) mem::ScopedCloneablePtr<namespace##::##CppType>;
%{
mem::ScopedCloneablePtr< namespace##::##CppType > makeScopedCloneable##PyType()
{
    return mem::ScopedCloneablePtr< namespace##::##CppType >(new namespace##::##CppType ());
}
%}

mem::ScopedCloneablePtr< namespace##::##CppType > makeScopedCloneable##PyType();
%enddef

%define SHARED(namespace, type)
SHARED_RENAME(namespace, type, type)
%enddef

%define SCOPED_COPYABLE(namespace, type)
SCOPED_COPYABLE_RENAME(namespace, type, type)
%enddef

/*
 * Same as above but for ScopedCloneablePtr
 */
%define SCOPED_CLONEABLE(namespace, type)
SCOPED_CLONEABLE_RENAME(namespace, type, type)
%enddef

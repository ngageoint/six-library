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

%module(package="coda") mt

%feature("autodoc","1");
 
%{
#include <mt/ThreadPlanner.h>
#include <import/except.h>
#include <mt/ThreadGroup.h>
#include <sys/Runnable.h>
%}
 
%import <except.i>
%import <sys.i>

%include <sys/Runnable.h>

%extend mt::ThreadPlanner
{

PyObject* getThreadInfo(size_t threadNum)
{
    size_t startElement;
    size_t numElementsThisThread;
    bool isValid = self->getThreadInfo(threadNum, startElement, numElementsThisThread);

    return Py_BuildValue("Nnn", PyBool_FromLong(isValid), startElement, numElementsThisThread);
}

}
 
%ignore mt::ThreadPlanner::getThreadInfo;
%include <mt/ThreadPlanner.h>

%extend mt::ThreadGroup
{


/* For SWIG purposes, this replaces the createThread default inside
 * mt::ThreadGroup.  This takes a runnable (otherwise, the function will throw)
 * and uses SWIG's ConvertPtr to steal the reference.  This is necessary
 * since our threading class expects an auto_ptr and will do a delete after
 * execution of the run() command -- leading to an eventual double delete.
 * This also has the bonus of dodging object lifetime issues in python -- since
 * otherwise an active reference must be kept in python while the thread runs
 * lest the garbage collector kick in
 */
void createThread(PyObject* runnable)
{
    sys::Runnable *n;
    if (SWIG_ConvertPtr(runnable, (void **) &n, SWIGTYPE_p_sys__Runnable, SWIG_POINTER_DISOWN) == -1)
    {
        throw except::Exception(Ctxt(
            "Invalid data type in createThread(expected sys::Runnable)"));
    }
    self->createThread(n);

}
}

%ignore mt::ThreadGroup::createThread;

%include <mt/ThreadGroup.h>


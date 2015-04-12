/* =========================================================================
 * This file is part of except-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * except-c++ is free software; you can redistribute it and/or modify
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


#ifndef __IMPORT_EXCEPT_H__
#define __IMPORT_EXCEPT_H__

/*!
 *  \file except.h
 *  \brief The except module is a base library for exceptions
 *
 *  Exceptions are fundamental to most modern C++ applications.
 *  The exception library contained herein uses the Java approach
 *  to defining exceptions.  The base class is Throwable, which defines
 *  the class of all such events worthy of unwinding the stack, and being
 *  caught in try/catch blocks.  Quite literally, a Throwable is something
 *  which, when produced, will be thrown.
 *
 *  Like in java, there are two sub-categories of Throwable:
 *  Exception classes, and Error classes.  Exceptions are considered to
 *  be types of Throwables that can be corrected once the state has been
 *  rolled back.  They, in other words, are essentially non-fatal errors.
 *  Errors are typically things that the developer would consider serious
 *  (constituting behavior worthy of death).
 *
 *  Generally speaking, when we use exceptions in this library,
 *  we throw our exception and we allow it to be caught by a surrounding
 *  try block.  Thrown exceptions unwind the stack until they are caught
 *  by what is known as a catch block (Each try block must be coupled
 *  with a catch block.  To catch, you simply need to give a parameter
 *  to the catch block.  A parameter should always be caught by reference,
 *  otherwise it will take on attributes only of the class that it is
 *  caught (the benefits of inheritance are lost then).
 *
 *  More specifically, in this library, we create an exception, usually
 *  taking a string or char* as the right-hand-side.  Typically, a higher
 *  level "handler" will construct this message for us (it may be a
 *  combination of developer and handler -- similar to perror).  We
 *  enforce, however, no such requirements in this package.
 *
 *
 *
 */

#include "except/Context.h"
#include "except/Trace.h"
#include "except/Throwable.h"
#include "except/Error.h"
#include "except/Exception.h"
#define EXCEPT_MAJOR_VERSION 0
#define EXCEPT_MINOR_VERSION 1
#define EXCEPT_MICRO_VERSION 0


#endif

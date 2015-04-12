/* =========================================================================
 * This file is part of sys-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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


#ifndef __DBG_H__
#define __DBG_H__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>

#if defined(__sgi) || defined(__sgi__)
#   include <stdarg.h>
#else
#   include <cstdarg>
#endif

#ifndef DEBUG_STREAM
#define DEBUG_STREAM stderr
#endif

/*! \file Dbg.h
 *  \brief Debgging macros and functions.
 *  
 *  Used primarily to trace and record information for debugging.
 *  By defining __DEBUG, these macros are turned on -- otherwise,
 *  they are ignored.
 *
 *  A quick macro explanation:
 *  \par __DEBUG
 *  Only die_printf will still function if __DEBUG is not defined.  All other
 *  functions and macros in this file will be turned off
 *  
 *  \par DEBUG_STREAM
 *  This is a FILE*.  you can define it easily to stderr or stdout.  Other
 *  definitions take some work
 *
 *  \par EVAL(X)
 *  Give this macro any variable and when defined, it will dump the name and
 *  current value of that variable in the format x=6, along with the file and
 *  line.  If __DEBUG_SHORTEN_EVAL is defined, it will not include file and
 *  line number:  sometimes that information can be tedious.
 *
 *  \par DUMP(T, X)
 *  Give this macro any variable and a type (%x, etc.) and it will write it 
 *  and its value with a file and line number.
 *
 *  \par HERE()
 *  Place this macro anywhere in the code, and when it is reached, it will
 *  print the file and line number to DEBUG_STREAM
 *
 *  \par TRACE(X)
 *  Place this macro anywhere in the code that you want to see, as-is.
 *  It will first print the code, and then evaluate it:
 *  TRACE(printf("hello\n"));
 *
 *  \par ASSERT_OR(ASSERTION, ELSE)
 *  This macro gives an alternative to quit.  ELSE could be a function to
 *  open emacs, for instance, with the file in question at the line number
 *  in question.
 */

namespace sys
{
/*!
 *  Prints to DEBUG_STREAM if __DEBUG is defined
 *  \param format
 *  \param ...  Put in any format value here
 */
void dbgPrintf(const char *format, ...);

/*!
 *  Prints to DEBUG_STREAM and kills the program
 *  \param format
 *  \param ...  Put in any format value here
 */
void diePrintf(const char *format, ...);
}

#define dbg_printf sys::dbgPrintf
#define die_printf sys::diePrintf
#define dbg_ln(STR) dbg_printf("[%s, %d]: '%s'\n", __FILE__, __LINE__, STR)

#ifdef __DEBUG

#ifndef __DEBUG_SHORTEN_EVAL
 #define EVAL(X) std::cout << '(' << __FILE__ << ',' <<__LINE__ << ") <EVAL> "#X"=" << X << std::endl
#else
 #define EVAL(X) std::cout << "<EVAL> "#X"=" << X << std::endl
#endif
 #define DUMP(T, X) printf("<DUMP> (%s:%d): "#X"="#T"\n", __FILE__, __LINE__, X)
 #define HERE() printf("<HERE> (%s:%d)\n", __FILE__, __LINE__)
 #define TRACE(X) printf("<TRACE> (%s:%d): "#X"\n", __FILE__, __LINE__); X
 #define ASSERT_OR(ASSERTION, ELSE) { if (ASSERTION) { 1; } else { dbg_printf("(%s, %d): Assertion failed: "#ASSERTION"\n", __FILE__, __LINE__); ELSE; exit(EXIT_FAILURE); } }

#else
 #define EVAL(X)    1
 #define DUMP(T, X) 1
 #define HERE()     1
 #define TRACE(X) X
 #define ASSERT_OR(A, E) 1
#endif

#endif // __DBG_H__

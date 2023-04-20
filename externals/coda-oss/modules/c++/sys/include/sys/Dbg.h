/* =========================================================================
 * This file is part of sys-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2021, Maxar Technologies, Inc.
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


#ifndef CODA_OSS_sys_Dbg_h_INCLUDED_
#define CODA_OSS_sys_Dbg_h_INCLUDED_
#pragma once

#include "config/Exports.h"
#include "config/disable_compiler_warnings.h"

// A "debug" build has debugging symbols, detailed call stacks, minimal optimization, STL validation, etc.
// A "release" build is likely to "run fast" and be "shipped;" it might lack much of what is in a "debug" build.
#ifndef CODA_OSS_DEBUG
    #if defined(_MSC_VER)
        // https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=msvc-160
        #if defined(_DEBUG) && _DEBUG // "Defined as 1 ... . Otherwise, undefined."
            #ifdef NDEBUG
                #error "NDEBUG #define'd with _DEBUG"
            #endif
            #define CODA_OSS_DEBUG 1
        #else
            #define CODA_OSS_DEBUG 0
        #endif
    #endif // _MSC_VER

    // GCC has a "neither" mode with no flags; no -O (optimization) and no -g (debugging);
    // that doesn't seem very useful, so try to figure out something that makes sense.
    #if defined(__GNUC__)
        // https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html#Common-Predefined-Macros
        // https://gcc.gnu.org/onlinedocs/libstdc++/manual/debug_mode_using.html#debug_mode.using.mode
        #if __OPTIMIZE__ && __NO_INLINE__
            #error "Both __OPTIMIZE__ and __NO_INLINE__"
        #endif
        #if __OPTIMIZE__ && defined(_GLIBCXX_DEBUG)
            #error "Both __OPTIMIZE__ and _GLIBCXX_DEBUG"
        #endif

        #if defined(_GLIBCXX_DEBUG)
            #ifdef NDEBUG
                #error "NDEBUG #define''d with _GLIBCXX_DEBUG."
            #endif
            #define CODA_OSS_DEBUG 1
        #elif __NO_INLINE__
            #ifdef NDEBUG
                #define CODA_OSS_DEBUG 0
            #else
                #define CODA_OSS_DEBUG 1
           #endif
        #elif __OPTIMIZE__
            #ifndef NDEBUG
                //#error "NDEBUG should be #define'd with __OPTIMIZE__"
            #endif
            #define CODA_OSS_DEBUG 0
        #else
            #error "Can't #define CODA_OSS_DEBUG for __GNUC__."
        #endif
    #endif // __GNUC__

    // TODO: ... other compilers ...

    #ifndef CODA_OSS_DEBUG
        //#error "Can't #define CODA_OSS_DEBUG for this compiler."
        #ifdef NDEBUG  // https://en.cppreference.com/w/c/error/assert
            #define CODA_OSS_DEBUG 0  // NDEBUG = "No DEBUG"
        #else
            #define CODA_OSS_DEBUG 1
        #endif  // NDEBUG
    #endif  // CODA_OSS_DEBUG
#endif // CODA_OSS_DEBUG
#ifndef CODA_OSS_DEBUG
    #error CODA_OSS_DEBUG is not set.
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <cstdarg>

namespace sys
{

    // compile-time for Dbg.h
    constexpr auto debug = CODA_OSS_DEBUG ? true : false;
    constexpr auto release = !debug;

    // build-time for Dbg.cpp; may (although shouldn't) be different than above.
    // C++ says little about debug/release/optimize/etc. (there's NDEBUG inherited from C);
    // but mixing is likely to cause all kinds of problems.
    CODA_OSS_API bool debug_build();
    inline bool release_build()
    {
        return !debug_build();
    }
}

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
// Keep __DEBUG for existing code/scripts; but shouldn't be used.
#ifndef CODA_OSS_debugging
    #ifdef __DEBUG
        #define CODA_OSS_debugging 1
        // or ... use the value of CODA_OSS_DEBUG ?
        //#define CODA_OSS_debugging CODA_OSS_DEBUG
    #else
        // or here ... ?
        //#define CODA_OSS_debugging CODA_OSS_DEBUG
        #define CODA_OSS_debugging 0
    #endif
#endif // CODA_OSS_debugging

namespace sys
{
constexpr bool debugging = CODA_OSS_debugging ? true : false;
}

namespace sys
{
/*!
 *  Prints to DEBUG_STREAM if __DEBUG is defined
 *  \param format
 *  \param ...  Put in any format value here
 */
CODA_OSS_API void dbgPrintf(const char *format, ...);

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

#if CODA_OSS_debugging

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

#endif // CODA_OSS_sys_Dbg_h_INCLUDED_

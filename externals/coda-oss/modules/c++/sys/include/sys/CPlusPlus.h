/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
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

#ifndef CODA_OSS_sys_CPlusPlus_h_INCLUDED_
#define CODA_OSS_sys_CPlusPlus_h_INCLUDED_
#pragma once

#define CODA_OSS_cplusplus __cplusplus
#if CODA_OSS_cplusplus < 201103L  // We need at least C++11
    #undef CODA_OSS_cplusplus  // oops...try to fix

    // MSVC only sets __cplusplus >199711L with the /Zc:__cplusplus command-line option.
    // https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/
    #if defined(_MSVC_LANG)
    // https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=msvc-160
    // "Defined as an integer literal that specifies the C++ language standard targeted by the compiler."
    #define CODA_OSS_cplusplus _MSVC_LANG
    #endif // _MSVC_LANG

    #if defined(__GNUC__)
    // ... similar things needed for other compilers ... ?
    #endif // __GNUC__

#endif // CODA_OSS_cplusplus

// Define a few macros as that's less verbose than testing against a version number
#define CODA_OSS_cpp11 (CODA_OSS_cplusplus >= 201103L)
#define CODA_OSS_cpp14 (CODA_OSS_cplusplus >= 201402L)
#define CODA_OSS_cpp17 (CODA_OSS_cplusplus >= 201703L)
#define CODA_OSS_cpp20 (CODA_OSS_cplusplus >= 202002L)
#define CODA_OSS_cpp23 0

// static_assert() is C++11
#if !defined(CODA_OSS_cpp11) || (CODA_OSS_cpp11 <= 0)
#error "Must compile with C++11 or greater."
#endif

// We've got various "replacements" (to a degree) for C++ library functionality
// only available in later releases.  Adding these names to "std" is technically
// forbidden, but it makes for fewer (eventual) changes in client code.
//
// You can bring these into your code by #include'ng the file from "std"; e.g.,
//    #include <std/span> // std::span

#endif // CODA_OSS_sys_CPlusPlus_h_INCLUDED_

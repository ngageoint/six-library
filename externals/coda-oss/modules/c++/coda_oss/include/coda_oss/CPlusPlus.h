/* =========================================================================
 * This file is part of coda_oss-c++
 * =========================================================================
 *
 * (C) Copyright 2021, 2022, Maxar Technologies, Inc.
 *
 * coda_oss-c++ is free software; you can redistribute it and/or modify
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

#ifndef CODA_OSS_coda_oss_CPlusPlus_h_INCLUDED_
#define CODA_OSS_coda_oss_CPlusPlus_h_INCLUDED_
#pragma once

#ifdef CODA_OSS_cplusplus 
    #error "CODA_OSS_cplusplus already #define'd."
#endif

#ifndef __cplusplus
    #error "Only C++ compilation is supported."
#endif
#define CODA_OSS_cplusplus __cplusplus

#if CODA_OSS_cplusplus < 201103L  // We need at least C++11
    #undef CODA_OSS_cplusplus  // oops...try to fix

    // MSVC only sets __cplusplus >199711L with the /Zc:__cplusplus command-line option.
    // https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/
    #if defined(_MSVC_LANG)
    // https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=msvc-160
    // "Defined as an integer literal that specifies the C++ language standard targeted by the compiler."
    #define CODA_OSS_cplusplus _MSVC_LANG
    #elif defined(_MSC_VER)
	#error "_MSVC_LANG should be #define'd."
    #endif // _MSVC_LANG

    #if defined(__GNUC__)
    #endif // __GNUC__
#endif // CODA_OSS_cplusplus
#if CODA_OSS_cplusplus < 202002L
    // oops ... try to fix
    #if defined(__GNUC__) && (__cplusplus >= 201709L) // note > C++ 17 of 201703L
        // Enough C++20 for our needs
        #undef CODA_OSS_cplusplus
        #define CODA_OSS_cplusplus 202002L
     #endif
#endif  // CODA_OSS_cplusplus

#if CODA_OSS_cplusplus < 201103L  
#error "Must compile with C++11 or greater."
#endif

// Define a few macros as that's less verbose than testing against a version number
#define CODA_OSS_cpp11 (CODA_OSS_cplusplus >= 201103L)
#define CODA_OSS_cpp14 (CODA_OSS_cplusplus >= 201402L)
#define CODA_OSS_cpp17 (CODA_OSS_cplusplus >= 201703L)
#define CODA_OSS_cpp20 (CODA_OSS_cplusplus >= 202002L)
#define CODA_OSS_cpp23 0

#endif // CODA_OSS_coda_oss_CPlusPlus_h_INCLUDED_

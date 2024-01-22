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

#pragma once
#ifndef CODA_OSS_coda_oss_CPlusPlus_h_INCLUDED_
#define CODA_OSS_coda_oss_CPlusPlus_h_INCLUDED_

#ifdef CODA_OSS_cplusplus 
    #error "CODA_OSS_cplusplus already #define'd."
#endif

#ifndef __cplusplus
    #error "Only C++ compilation is supported."
#endif
#define CODA_OSS_cplusplus __cplusplus

// There are now enough C++ versions that it's a pain to keep track of the values
// https://en.cppreference.com/w/cpp/preprocessor/replace#Predefined_macros
#define CODA_OSS_cplusplus11 201103L
#define CODA_OSS_cplusplus14 201402L
#define CODA_OSS_cplusplus17 201703L
#define CODA_OSS_cplusplus20 202002L
#define CODA_OSS_cplusplus23 202302L

#if CODA_OSS_cplusplus < CODA_OSS_cplusplus11
    #undef CODA_OSS_cplusplus  // oops...try to fix

    // MSVC only sets __cplusplus >199711L with the /Zc:__cplusplus command-line option.
    // https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/
    #if defined(_MSC_VER)
        #if defined(_MSVC_LANG)
            // https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=msvc-160
            // "Defined as an integer literal that specifies the C++ language standard targeted by the compiler."
            #define CODA_OSS_cplusplus _MSVC_LANG
        #else
	        #error "_MSVC_LANG should be #define'd."
        #endif
    #endif // _MSC_VER

    #if defined(__GNUC__)
    #endif // __GNUC__

    #if defined(__INTEL_COMPILER)
    #endif  // __INTEL_COMPILER
#endif // CODA_OSS_cplusplus

#if CODA_OSS_cplusplus < CODA_OSS_cplusplus20
    #if defined(__GNUC__) && (__cplusplus >= 201709L)  // note > C++ 17 of 201703L
        // Enough C++20 for our much of our needs ...
        // at least with std/ and coda_oss/ library support.
        #undef CODA_OSS_cplusplus
        #define CODA_OSS_cplusplus CODA_OSS_cplusplus20
    #endif
#endif  // CODA_OSS_cplusplus

// Define a few macros as that's less verbose than testing against a version number
#define CODA_OSS_cpp11 (CODA_OSS_cplusplus >= CODA_OSS_cplusplus11)
#define CODA_OSS_cpp14 (CODA_OSS_cplusplus >= CODA_OSS_cplusplus14)
#define CODA_OSS_cpp17 (CODA_OSS_cplusplus >= CODA_OSS_cplusplus17)
#define CODA_OSS_cpp20 (CODA_OSS_cplusplus >= CODA_OSS_cplusplus20)
#define CODA_OSS_cpp23 (CODA_OSS_cplusplus >= CODA_OSS_cplusplus23)

#if !CODA_OSS_cpp17
#error "Must compile with C++17 or greater."
#endif

// Get feature-testing macros: https://en.cppreference.com/w/cpp/feature_test
#if CODA_OSS_cpp17 // C++17 for __has_include
    #if !CODA_OSS_cpp20
        // "error C4996: '...': warning STL4036: <ciso646> is removed in C++20. You can define _SILENCE_CXX20_CISO646_REMOVED_WARNING or _SILENCE_ALL_CXX20_DEPRECATION_WARNINGS to suppress this warning."

        // https://en.cppreference.com/w/cpp/header/version
        // "Prior to C++20, including <ciso646> is sometimes used for this purpose."
        #if __has_include(<ciso646>)
            #include <ciso646>
        #endif
    #endif // !CODA_OSS_cpp20

    #if __has_include(<version>)
        #include <version>
    #endif
#endif // CODA_OSS_cpp17

#endif // CODA_OSS_coda_oss_CPlusPlus_h_INCLUDED_

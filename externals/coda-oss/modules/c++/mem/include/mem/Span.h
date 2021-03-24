/* =========================================================================
 * This file is part of mem-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * mem-c++ is free software; you can redistribute it and/or modify
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
#ifndef CODA_OSS_mem_Span_h_INCLUDED_
#define CODA_OSS_mem_Span_h_INCLUDED_
#pragma once

#include "sys/CPlusPlus.h"
#include "gsl/gsl_span_.h"

#ifndef CODA_OSS_AUGMENT_std_span_
    #define CODA_OSS_AUGMENT_std_span_ CODA_OSS_AUGMENT_std_namespace // maybe use our own
#endif
#ifndef CODA_OSS_DEFINE_std_span_
    #if CODA_OSS_cpp20
        #define CODA_OSS_DEFINE_std_span_ -1  // OK to #include <>, below
    #else
        #define CODA_OSS_DEFINE_std_span_ CODA_OSS_AUGMENT_std_span_
    #endif  // CODA_OSS_cpp20
#endif // CODA_OSS_DEFINE_std_span_

#if CODA_OSS_DEFINE_std_span_ == 1
    namespace std // This is slightly uncouth: we're not supposed to augment "std".
    {
        template<typename T>
        using span = gsl::span<T>;
    }
    #define CODA_OSS_lib_span 202002
#elif CODA_OSS_DEFINE_std_span_ == -1  // set above
    #include <span>
    #define CODA_OSS_lib_span 202002
#endif  // CODA_OSS_DEFINE_std_span_

// Note that C++20 doesn't need "make_" helpers anymore!
//
// Since that's the easiest way to use span<> pre-C++20, and since we can't
// add the nice C++20 way of using span<> w/o actuallly having C++20,
// don't provide the option to add mem::Span to std.  When clients can build
// with C++20, they'll want to get rid of make_Span and use std::span
// directly.
//
// Also note that pre-C++20 routines such std::make_optional and
// std::make_unique ARE still available since std::optional and
// std::unique_ptr existed before C++20.
namespace coda_oss
{
    #if CODA_OSS_lib_span
     template <typename T>
     using span = std::span<T>;
    #else
     template <typename T>
     using span = gsl::span<T>;
    #endif
}

#endif  // CODA_OSS_mem_Span_h_INCLUDED_

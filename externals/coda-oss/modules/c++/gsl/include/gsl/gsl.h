/* =========================================================================
 * This file is part of gsl-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * gsl-c++ is free software; you can redistribute it and/or modify
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

#ifndef CODA_OSS_gsl_h_INCLUDED_
#define CODA_OSS_gsl_h_INCLUDED_

#pragma once

#include "sys/Conf.h"

// Need a fairly decent C++ compiler to use the real GSL
#if CODA_OSS_cpp14
#define CODA_OSS_use_gsl_ 1
#endif

#if !CODA_OSS_use_gsl_

#include "gsl/Gsl_.h" // our own "fake" GSL

#else

#include "gsl/gsl_algorithm"	// copy
#include "gsl/gsl_assert"		// Ensures/Expects
#include "gsl/gsl_byte"			// byte
#include "gsl/gsl_util"			// finally()/narrow()/narrow_cast()...
#include "gsl/multi_span"		// multi_span, strided_span...
#include "gsl/pointers"			// owner, not_null
#include "gsl/span"				// span
#include "gsl/string_span"		// zstring, string_span, zstring_builder...

#endif

#ifndef CODA_OSS_DEFINE_std_span_
    #if CODA_OSS_cpp20 && __has_include(<span>)  // __has_include is C++17
        #define CODA_OSS_DEFINE_std_span_ -1  // OK to #include <>, below
    #else
        #define CODA_OSS_DEFINE_std_span_ CODA_OSS_AUGMENT_std_namespace  // maybe use our own
    #endif // CODA_OSS_cpp20
#endif  // CODA_OSS_DEFINE_std_span_

#if CODA_OSS_DEFINE_std_span_ == 1
    namespace std // This is slightly uncouth: we're not supposed to augment "std".
    {
        template<typename T>
        using span = gsl::span<T>;
    }
#elif CODA_OSS_DEFINE_std_span_ == -1  // set above
    #include <span>
#endif // CODA_OSS_DEFINE_std_span_

#endif  // CODA_OSS_gsl_h_INCLUDED_

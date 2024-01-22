/* =========================================================================
 * This file is part of coda_oss-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * © Copyright 2023, Maxar Technologies, Inc.
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

#include "coda_oss/CPlusPlus.h"

// This should always work ... it's in a `details` namespace
#include "coda_oss/mdspan_.h"

// This logic needs to be here rather than <std/mdspan> so that `coda_oss::mdspan` will
// be the same as `std::mdspan`.
#ifndef CODA_OSS_HAVE_std_mdspan_
    #define CODA_OSS_HAVE_std_mdspan_ 0  // assume no <mdspan>
#endif
#ifndef CODA_OSS_HAVE_experimental_mdspan_
    #define CODA_OSS_HAVE_experimental_mdspan_ 0  // assume no std::experimental::mdspan
#endif
#if CODA_OSS_cpp17 // __has_include
    #if __has_include(<mdspan>) // <mdspan> not until C++23
        #include <mdspan>
        #undef CODA_OSS_HAVE_std_mdspan_
        #define CODA_OSS_HAVE_std_mdspan_ 1  // provided by the implementation, probably C++23
    #endif
   
    #if CODA_OSS_cpp20  // Can't even #include this file with older C++14/17 compilers! :-(
        // Put this in a __has_include so that it's optional.  Our simple implemtnation works
        // for our needs, and this brings along a lot of code that our older compilers don't
        // like. By the time we need more functionality, maybe we'll be using C++23?
        //
        // Until then, having this available allows checking our implementation against
        // something much more real. https://github.com/kokkos/mdspan
        #if __has_include("coda_oss/experimental/mdspan")
            #include "coda_oss/experimental/mdspan"
            #undef CODA_OSS_HAVE_experimental_mdspan_
            #define CODA_OSS_HAVE_experimental_mdspan_ 1  // provided coda_oss/experimental/mdspan
        #endif
    #endif
#endif // CODA_OSS_cpp17

namespace coda_oss
{
    #if CODA_OSS_HAVE_std_mdspan_
        using std::mdspan;
        using std::dextents;
    #elif CODA_OSS_HAVE_experimental_mdspan_
        using std::experimental::mdspan;
        using std::experimental::dextents;
    #else
        using details::mdspan;
        using details::dextents;
    #endif 
}


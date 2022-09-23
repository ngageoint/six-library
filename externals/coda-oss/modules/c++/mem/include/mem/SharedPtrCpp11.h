/* =========================================================================
 * This file is part of mem-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2018, MDA Information Systems LLC
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

#ifndef CODA_OSS_mem_SharedPtrCpp11_h_INCLUDED_
#define CODA_OSS_mem_SharedPtrCpp11_h_INCLUDED_
#pragma once

#include <memory>
#include <type_traits>

#include "coda_oss/memory.h"
#include "sys/CPlusPlus.h"

namespace mem
{
// This won't work everywhere since C++11's std::unique_ptr<> often requires
// "&&" and std::move. But for member data and the like it can reduce some
// boiler-plate code; note that it's often possible to just use std::unique_ptr
// directly.  This is mostly needed to support existing interfaces.
#if CODA_OSS_cpp17  // std::auto_ptr removed in C++17
    #if defined(CODA_OSS_no_autoptr) && (!CODA_OSS_no_autoptr)
        #error "std::auto_ptr was removed in C++17."
    #endif
    #define CODA_OSS_autoptr_is_std 0  // mem::auto_ptr != std::auto_ptr
#else // C++11 or C++14 still have std::auto_ptr, but it's depricated
    #ifdef CODA_OSS_no_autoptr  // don't use std::auto_ptr even if it's available
        #define CODA_OSS_autoptr_is_std 0  // mem::auto_ptr != std::auto_ptr
    #else
        #define CODA_OSS_autoptr_is_std 1  // mem::auto_ptr == std::auto_ptr
    #endif
#endif
 template <typename T> using auto_ptr =
#if CODA_OSS_autoptr_is_std
   std::auto_ptr<T>;
#else
    std::unique_ptr<T>;
#endif

// Pretty much give-up on mem::SharedPtr as it's too hard to get something that will
// compile with all the different compilers; let somebody else worry about that
// via std::shared_ptr.  The only code change is use_count() instead of getCount(),
// and that's mostly used in unit-tests.
template<typename T>
using SharedPtr = std::shared_ptr<T>;
} // namespace mem

// try to make code changes a tiny bit easier?
template<typename T>
inline long getCount(const std::shared_ptr<T>& p) noexcept // be sure const& so that calling doesn't increment!
{
    return p.use_count();
}

#endif // CODA_OSS_mem_SharedPtrCpp11_h_INCLUDED_

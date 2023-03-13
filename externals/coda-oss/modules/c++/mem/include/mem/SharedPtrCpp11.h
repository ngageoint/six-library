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
// This won't work everywhere since C++11's std::unique_ptr<> often requries
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
#if !CODA_OSS_enable_mem_SharedPtr
template<typename T>
using SharedPtr = std::shared_ptr<T>;
#else
/*!
 *  \class SharedPtr
 *  \brief This is a derived class of std::shared_ptr. The purpose of this
 *         class is to provide backwards compatibility in systems that do
 *         not have C++11 support.
 *         Because this inherits from std::shared_ptr it can be directly
 *         passed into interfaces requiring std::shared_ptr or legacy
 *         interfaces.
 *         For future work, prefer std::shared_ptr when possible.
 *
 *         WARNING: std::shared_ptr<T>* foo = new SharedPtr<T> will leak!
 */
template <class T>
class SharedPtr : public std::shared_ptr<T>
{
public:
    SharedPtr() = default;
    ~SharedPtr() = default;

    using std::shared_ptr<T>::shared_ptr;

    using std::shared_ptr<T>::reset;

    SharedPtr(const SharedPtr&) = default;
    SharedPtr& operator=(const SharedPtr&) = default;
    SharedPtr(SharedPtr&&) = default;
    SharedPtr& operator=(SharedPtr&&) = default;

    template<typename OtherT>
    SharedPtr(const std::shared_ptr<OtherT>& ptr)
    {
        *this = ptr;
    } 
    template<typename OtherT>
    SharedPtr& operator=(const std::shared_ptr<OtherT>& ptr)
    {
      std::shared_ptr<T>& base = *this;
      base = ptr;
      return *this;
    }

    template <typename OtherT>
    explicit SharedPtr(std::unique_ptr<OtherT>&& ptr) :
        std::shared_ptr<T>(ptr.release())
    {
    }

    void reset(std::unique_ptr<T>&& scopedPtr)
    {
        std::shared_ptr<T>::reset(scopedPtr.release());
    }

    #if CODA_OSS_autoptr_is_std // std::auto_ptr removed in C++17
    // The base class only handles auto_ptr<T>&&
    explicit SharedPtr(mem::auto_ptr<T> ptr) :
        std::shared_ptr<T>(ptr.release())
    {
    }

    // The base class only handles auto_ptr<T>&&
    template <typename OtherT>
    explicit SharedPtr(mem::auto_ptr<OtherT> ptr) :
        std::shared_ptr<T>(ptr.release())
    {
    }

    void reset(mem::auto_ptr<T> scopedPtr)
    {
        std::shared_ptr<T>::reset(scopedPtr.release());
    }
    #endif

    // Implemented to support the legacy SharedPtr. Prefer use_count.
    long getCount() const
    {
        return std::shared_ptr<T>::use_count();
    }
};
#endif // CODA_OSS_enable_mem_SharedPtr
} // namespace mem

// try to make code changes a tiny bit easier?
template<typename T>
inline long getCount(const std::shared_ptr<T>& p) noexcept // be sure const& so that calling doesn't increment!
{
    return p.use_count();
}

#endif // CODA_OSS_mem_SharedPtrCpp11_h_INCLUDED_

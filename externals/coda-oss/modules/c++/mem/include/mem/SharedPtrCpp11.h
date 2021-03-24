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

#ifndef __MEM_SHARED_PTR_CPP_11_H__
#define __MEM_SHARED_PTR_CPP_11_H__
#pragma once

#include <memory>
#include <type_traits>

#include "sys/Conf.h"

namespace mem
{
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
    SharedPtr() : std::shared_ptr<T>()
    {
    }

    using std::shared_ptr<T>::shared_ptr;

    using std::shared_ptr<T>::reset;

    explicit SharedPtr(std::unique_ptr<T>&& ptr) :
        std::shared_ptr<T>(ptr.release())
    {
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

    #if !CODA_OSS_cpp17  // std::auto_ptr removed in C++17
    // The base class only handles auto_ptr<T>&&
    explicit SharedPtr(std::auto_ptr<T> ptr) :
        std::shared_ptr<T>(ptr.release())
    {
    }

    // The base class only handles auto_ptr<T>&&
    template <typename OtherT>
    explicit SharedPtr(std::auto_ptr<OtherT> ptr) :
        std::shared_ptr<T>(ptr.release())
    {
    }

    void reset(std::auto_ptr<T> scopedPtr)
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

// This won't work everywhere since C++11's std::unique_ptr<> often requries "&&" and std::move.
// But for member data and the like it can reduce some boiler-plate code; note that it's often
// possible to just use std::unique_ptr directly.  This is mostly needed to support existing interfaces.
#if !CODA_OSS_cpp17  // std::auto_ptr removed in C++17
template<typename T>
using auto_ptr = std::auto_ptr<T>;
#else
template <typename T>
using auto_ptr = std::unique_ptr<T>;
#endif

// C++11 inadvertently ommitted make_unique; provide it here. (Swiped from <memory>.)
namespace make
{
// Using a nested namespace in case somebody does both
// "using namespace std" and "using namespace mem".

template <typename T, typename... TArgs, typename std::enable_if<!std::is_array<T>::value, int>::type = 0>
std::unique_ptr<T> unique(TArgs&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<TArgs>(args)...));
}

template <typename T, typename std::enable_if<std::is_array<T>::value &&  std::extent<T>::value == 0, int>::type = 0>
std::unique_ptr<T> unique(size_t size)
{
    using element_t = typename std::remove_extent<T>::type;
    return std::unique_ptr<T>(new element_t[size]());
}

template <typename T, typename... TArgs, typename std::enable_if<std::extent<T>::value != 0, int>::type = 0>
void unique(TArgs&&...) = delete;

#define CODA_OSS_mem_make_unique 201304L  // c.f., __cpp_lib_make_unique

}  // namespace make
} // namespace mem

#endif

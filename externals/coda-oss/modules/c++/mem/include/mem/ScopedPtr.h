/* =========================================================================
 * This file is part of mem-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2021, Maxar Technologies, Inc.
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

#ifndef CODA_OSS_mem_ScopedPtr_h_INCLUDED_
#define CODA_OSS_mem_ScopedPtr_h_INCLUDED_
#pragma once

#include <cstddef>
#include <std/memory>
#include <type_traits>

#include "sys/Conf.h"
#include "mem/SharedPtr.h"

namespace mem
{
/*!
 *  \class ScopedPtr
 *  \brief This class provides RAII for object allocations via new.  It is a
 *         light wrapper around std::unique_ptr and has the same semantics
 *         except that the copy constructor and assignment operator are deep
 *         copies (by using T's clone() method) rather than transferring
 *         ownership.
 *
 *         This is useful for cases where you have a class which has a member
 *         variable that's dynamically allocated and you want to provide a
 *         valid copy constructor / assignment operator.  With raw pointers or
 *         std::unique_ptr's, you'll have to write the copy constructor /
 *         assignment operator for this class - this is tedious and
 *         error-prone since you need to include all the members in the class.
 *         Using ScopedCloneablePtr's instead, the compiler-generated copy
 *         constructor and assignment operator for your class will be correct
 *         (if all the other member variables are POD or have correct
 *         copy constructors / assignment operators).
 */
template <typename T, typename TCopyIsClone>
class ScopedPtr
{
    std::unique_ptr<T> mPtr;

    void duplicate(const T& from, std::true_type)
    {
        reset(from.clone());    
    }
    void duplicate(const T& from, std::false_type)
    {
        reset(std::make_unique<T>(from));
    }

public:
    explicit ScopedPtr(T* ptr = nullptr)
    {
        reset(ptr);
    }

    explicit ScopedPtr(std::unique_ptr<T>&& ptr)
    {
        reset(std::move(ptr));
    }

    ScopedPtr(const ScopedPtr& rhs)
    {
        *this = rhs;
    }

    const ScopedPtr& operator=(const ScopedPtr& rhs)
    {
        if (this != &rhs)
        {
            auto rhs_ptr = rhs.get();
            if (rhs_ptr != nullptr)
            {
                duplicate(*rhs_ptr, TCopyIsClone());
            }
            else
            {
                reset();
            }
        }

        return *this;
    }

    ScopedPtr(ScopedPtr&&) = default;
    ScopedPtr& operator=(ScopedPtr&&) = default;

    bool operator==(const ScopedPtr& rhs) const noexcept
    {
        auto ptr = get();
        auto rhs_ptr = rhs.get();
        if (ptr == nullptr && rhs_ptr == nullptr)
        {
            return true;
        }

        if (ptr == nullptr || rhs_ptr == nullptr)
        {
            return false;
        }

        return *ptr == *rhs_ptr;
    }

    bool operator!=(const ScopedPtr& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    explicit operator bool() const noexcept
    {
        return get() == nullptr ? false : true;
    }

    T* get() const noexcept
    {
        return mPtr.get();
    }

    T& operator*() const
    {
        return *get();
    }

    T* operator->() const noexcept
    {
        return get();
    }

    void reset(T* ptr = nullptr)
    {
        mPtr.reset(ptr);
    }

    void reset(std::unique_ptr<T>&& ptr)
    {
        mPtr = std::move(ptr);
    }
};
}

#endif  // CODA_OSS_mem_ScopedPtr_h_INCLUDED_

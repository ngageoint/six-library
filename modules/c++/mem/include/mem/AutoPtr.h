/* =========================================================================
 * This file is part of mem-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2018, MDA Information Systems LLC
 * (C) Copyright 2022, Maxar Technologies, Inc.
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

#pragma once
#ifndef CODA_OSS_mem_AutoPtr_h_INCLUDED_
#define CODA_OSS_mem_AutoPtr_h_INCLUDED_

#include <memory>

namespace mem
{
// std::auto_ptr (deprecated in C++17) is copyable while
// std::unique_ptr (new in C++11) isn't.  While unique_ptr
// is right, making the change doesn't always work well with
// legacy code.  Using shared_ptr instead of unique_ptr is
// copyable, but that changes semantics: the "old" copied-from
// object is still valid.
//
// Thus, this class to help make the transition easier.
// Using (very) sparingly!

template<typename T>
class AutoPtr final
{
    std::unique_ptr<T> ptr_;

public:
    // https://en.cppreference.com/w/cpp/memory/auto_ptr/auto_ptr
    explicit AutoPtr(T* p = nullptr) noexcept : ptr_(p)
    {
    }

    AutoPtr& operator=(AutoPtr& r) noexcept
    {
        reset(r.release());
        return *this;
    }
    AutoPtr(AutoPtr& r) noexcept
    {
        *this = r;
    }
    AutoPtr& operator=(const AutoPtr&) = delete; // can't change a "const" object
    AutoPtr(const AutoPtr&) = delete;

    ~AutoPtr() = default;
    AutoPtr(AutoPtr&&) = default;
    AutoPtr& operator=(AutoPtr&&) = default;

    template<typename U>
    AutoPtr& operator=(std::unique_ptr<U>&& p) noexcept
    {
        ptr_ = std::move(p);
        return *this;
    }
    template <typename U>
    AutoPtr(std::unique_ptr<U>&& p) noexcept
    {
        *this = std::move(p);
    }

    template<typename TAutoPtr> // std::auto_ptr can cause deprecated warnings
    AutoPtr& assign(TAutoPtr p) noexcept
    {
        ptr_.reset(p.release());
        return *this;
    }
    template<typename TAutoPtr> // std::auto_ptr can cause deprecated warnings
    AutoPtr& operator=(TAutoPtr p) noexcept
    {
        return assign(p);
    }
    template<typename TAutoPtr> // std::auto_ptr can cause deprecated warnings
    AutoPtr(TAutoPtr p) noexcept
    {
        *this = assign(p);
    }


    T* get() const noexcept
    {
        return ptr_.get();
    }

    T* release() noexcept
    {
        return ptr_.release();
    }

    template<typename U>
    void reset(U* p = nullptr) noexcept
    {
        ptr_.reset(p);
    }

    T& operator*() const noexcept
    {
        return *get();
    }
    T* operator->() const noexcept
    {
        return get();
    }

    operator std::unique_ptr<T>& () { return ptr_; }
    operator const std::unique_ptr<T>& () const { return ptr_; }
    
};

} // namespace mem

#endif // CODA_OSS_mem_AutoPtr_h_INCLUDED_

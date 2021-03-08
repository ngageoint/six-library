/* =========================================================================
 * This file is part of mem-c++
 * =========================================================================
 *
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
#ifndef CODA_OSS_mem_Span__h_INCLUDED_
#define CODA_OSS_mem_Span__h_INCLUDED_
#pragma once

#include <stddef.h>
#include <assert.h>

namespace mem
{
// super-simple version of std::span
// https://en.cppreference.com/w/cpp/container/span
template <typename T>
struct Span final
{
    using size_type = size_t;
    using element_type = T;
    using pointer = T*;
    using reference = T&;

    Span() noexcept = default;
    Span(pointer p, size_type sz) noexcept : p_(p), sz_(sz)
    {
        assert(p != nullptr);
    }
    Span(const Span&) noexcept = default;

    template <typename TContainer>
    Span(TContainer& c) : Span(c.data(), c.size()) { }
    template <typename TContainer>
    Span(const TContainer& c) : Span(const_cast<TContainer&>(c)) { }

    // https://en.cppreference.com/w/cpp/container/span/data
    constexpr pointer data() const noexcept
    {
        return p_;
    }

    // https://en.cppreference.com/w/cpp/container/span/operator_at
    /*constexpr*/ reference operator[](size_type idx) const
    {
        assert(idx < size()); // prevents "constexpr" in C++11
        return data()[idx];
    }

    // https://en.cppreference.com/w/cpp/container/span/size
    constexpr size_type size() const noexcept
    {
        return sz_;
    }
    
    // https://en.cppreference.com/w/cpp/container/span/size_bytes
    constexpr size_type size_bytes() const noexcept
    {
        return sz_ * sizeof(element_type);
    }
    
    // https://en.cppreference.com/w/cpp/container/span/empty
    constexpr bool empty() const noexcept
    {
        return size() == 0;
    }

private:
    pointer p_ = nullptr;
    size_type sz_ = 0;
};

template <typename T>
inline Span<T> make_Span(T* d, size_t sz)
{
    return Span<T>(d, sz);
}

template <typename TContainer>
inline Span<typename TContainer::value_type> make_Span(TContainer& c)
{
    using value_type = typename TContainer::value_type;
    return Span<value_type>(c);
}
template <typename TContainer>
inline Span<typename TContainer::value_type> make_Span(const TContainer& c)
{
    return make_Span(const_cast<TContainer&>(c));
}
}

#endif  // CODA_OSS_mem_Span__h_INCLUDED_

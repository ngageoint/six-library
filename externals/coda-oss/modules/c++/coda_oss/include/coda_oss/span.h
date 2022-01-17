/* =========================================================================
 * This file is part of coda_oss-c++
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
#ifndef CODA_OSS_coda_oss_Span_h_INCLUDED_
#define CODA_OSS_coda_oss_Span_h_INCLUDED_
#pragma once

#include <assert.h>
#include <stddef.h>

#include "coda_oss/namespace_.h"
namespace coda_oss
{
namespace details
{
// super-simple version of std::span
// https://en.cppreference.com/w/cpp/container/span
template <typename T>
struct span final
{
    using size_type = size_t;
    using element_type = T;
    using pointer = T*;
    using reference = T&;

    span() noexcept = default;
    span(pointer p, size_type sz) noexcept : p_(p), sz_(sz)
    {
        assert(p != nullptr);
    }
    span(const span&) noexcept = default;

    template <typename TContainer>
    span(TContainer& c) : span(c.data(), c.size())
    {
    }
    template <typename TContainer>
    span(const TContainer& c) : span(const_cast<TContainer&>(c))
    {
    }

    // https://en.cppreference.com/w/cpp/container/span/data
    constexpr pointer data() const noexcept
    {
        return p_;
    }

    // https://en.cppreference.com/w/cpp/container/span/operator_at
    /*constexpr*/ reference operator[](size_type idx) const
    {
        assert(idx < size());  // prevents "constexpr" in C++11
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
}
}

// Need a fairly decent C++ compiler to use the real GSL.  This brings in more than 
// we really need for span (e.g., gsl::narrow()), but it keeps things simple.
#include "gsl/gsl.h"  // not gsl/span; need #pragma here to turn off warnings

#if defined(GSL_SPAN_H) // the above #include'd gsl/span
namespace coda_oss
{
template <typename T>
using span = gsl::span<T>;
}
#else // no gsl::span, use our own
namespace coda_oss
{
template <typename T>
using span = details::span<T>;
}
#endif  // GSL_SPAN_H

#define CODA_OSS_coda_oss_span 202002L  // c.f., __cpp_lib_span

#endif  // CODA_OSS_coda_oss_Span_h_INCLUDED_

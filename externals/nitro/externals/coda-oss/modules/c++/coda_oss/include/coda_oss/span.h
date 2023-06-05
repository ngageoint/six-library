/* =========================================================================
 * This file is part of coda_oss-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#ifndef CODA_OSS_coda_oss_span_h_INCLUDED_
#define CODA_OSS_coda_oss_span_h_INCLUDED_

#include <type_traits>

#include <type_traits>

#include "coda_oss/namespace_.h"
#include "coda_oss/span_.h"
#include "coda_oss/cstddef.h" // byte

// Need a fairly decent C++ compiler to use the real GSL.  This brings in more than 
// we really need for span (e.g., gsl::narrow()), but it keeps things simple.
#include "gsl/gsl.h"  // not gsl/span; need #pragma here to turn off warnings

namespace coda_oss
{
#if defined(GSL_SPAN_H) // the above #include'd gsl/span
	using gsl::span;
#else // no gsl::span, use our own
	using details::span;
#endif  // GSL_SPAN_H

// https://en.cppreference.com/w/cpp/container/span/as_bytes
template <typename T>
inline auto as_bytes(span<const T> s) noexcept
{
    // https://en.cppreference.com/w/cpp/types/is_trivially_copyable "... serialized to/from binary files ..."
    static_assert(std::is_trivially_copyable<T>::value, "must be 'trivially' copyable.");

    const void* const p_ = s.data();
    auto const p = static_cast<const byte*>(p_);
    return span<const byte>(p, s.size_bytes());
}
template <typename T>
inline auto as_bytes(span<T> s) noexcept
{
    const span<const T> s_(s.data(), s.size());
    return as_bytes(s_);
}
template <typename T>
inline span<byte> as_writable_bytes(span<T> s) noexcept
{
    // https://en.cppreference.com/w/cpp/types/is_trivially_copyable "... serialized to/from binary files ..."
    static_assert(std::is_trivially_copyable<T>::value, "must be 'trivially' copyable.");

    static_assert(!std::is_const<T>::value, "T cannot be 'const'");

    void* const p_ = s.data();
    auto const p = static_cast<byte*>(p_);
    return span<byte>(p, s.size_bytes());
}

}

#endif  // CODA_OSS_coda_oss_span_h_INCLUDED_

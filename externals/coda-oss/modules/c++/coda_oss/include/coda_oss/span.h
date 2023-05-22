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
#ifndef CODA_OSS_coda_oss_span_h_INCLUDED_
#define CODA_OSS_coda_oss_span_h_INCLUDED_
#pragma once

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
span<const byte> as_bytes(span<const T> s) noexcept
{
    const void* const p = s.data();
    return span<const byte>(static_cast<const byte*>(p), s.size_bytes());
}
template <typename T>
span<const byte> as_bytes(span<T> s) noexcept
{
    return as_bytes(span<const T>(s.data(), s.size()));
}
template <typename T>
span<byte> as_writable_bytes(span<T> s) noexcept
{
    void* const p = s.data();
    return span<byte>(static_cast<byte*>(p), s.size_bytes());
}

}

#endif  // CODA_OSS_coda_oss_span_h_INCLUDED_

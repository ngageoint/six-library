/* =========================================================================
 * This file is part of types-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2023, Maxar Technologies, Inc.
 *
 * types-c++ is free software; you can redistribute it and/or modify
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
#ifndef CODA_OSS_types_complex_h_INCLUDED_
#define CODA_OSS_types_complex_h_INCLUDED_

#include <stdint.h>

#include <complex>
#include <type_traits>

#include "config/disable_compiler_warnings.h"
#include "coda_oss/CPlusPlus.h"

namespace types
{
namespace details
{
/*!
 *  \class complex
 *  \brief Our own implementation of std::complex<T> for SIX and friends.
 *
 * `std::complex<TInt>` is no longer valid C++; provide a (partial) work-around.
 * See https://en.cppreference.com/w/cpp/numeric/complex for detals.
 * 
 * SIX (and others) mostly use `std::complex<short>` as a 
 * convenient package for two values; very little "complex math" is done
 * using integers.
 */
template<typename T>
struct complex final
{
    using value_type = T;
    static_assert(!std::is_floating_point<T>::value, "Use std::complex<T> for floating-point.");
    static_assert(std::is_signed<T>::value, "T should be a signed integer.");

    complex(value_type re = 0, value_type im = 0) : z{re, im} {}
    complex(const complex&) = default;
    complex& operator=(const complex&) = default;
    complex(complex&&) = default;
    complex& operator=(complex&&) = default;
    ~complex() = default;

    value_type real() const
    {
        return z[0];
    }
    void real(value_type value)
    {
        z[0] = value;
    }

    value_type imag() const
    {
        return z[1];
    }
    void imag(value_type value)
    {
        z[1] = value;
    }

private:
    value_type z[2]{0, 0};
};

CODA_OSS_disable_warning_push
#ifdef _MSC_VER
#pragma warning(disable: 4996) // '...': warning STL4037: The effect of instantiating the template std::complex for any type other than float, double, or long double is unspecified. You can define _SILENCE_NONFLOATING_COMPLEX_DEPRECATION_WARNING to suppress this warning.
#endif

template<typename T>
inline const std::complex<T>& cast(const complex<T>& z)
{
    // Getting different results with GCC vs MSVC :-(  So just use
    // std::complex<short> Assume by the time we're actually using C++23 with a
    // compiler that enforces this restriction, "something" will be different.
    const void* const pZ_ = &z;
    return *static_cast<const std::complex<T>*>(pZ_);
}

CODA_OSS_disable_warning_pop

// https://en.cppreference.com/w/cpp/numeric/complex/abs
template<typename T>
inline auto abs(const complex<T>& z)
{
    return abs(cast(z));
}

}

// Have the compiler pick between std::complex and details::complex
template<typename T>
using complex = std::conditional_t<std::is_floating_point<T>::value, std::complex<T>, details::complex<T>>;

static_assert(sizeof(std::complex<short>) == sizeof(complex<short>), "sizeof(sizeof(std::complex<short>) != sizeof(complex<short>)");

// Convenient aliases
using zfloat = complex<float>; // std::complex<float>
using zdouble = complex<double>; // std::complex<double>
//using zlong_double = complex<long double>; // std::complex<long double>

// Intentionally using somewhat cumbersome names
using zint8_t = complex<int8_t>;  // details:complex<int8_t>
using zint16_t = complex<int16_t>;  // details:complex<int16_t>
using zint32_t = complex<int32_t>;  // details::complex<int32_t>
using zint64_t = complex<int64_t>;  // details::complex<int64_t>

}

#endif  // CODA_OSS_types_complex_h_INCLUDED_

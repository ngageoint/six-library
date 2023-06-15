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
#include <iostream>

#include "config/disable_compiler_warnings.h"
#include "coda_oss/CPlusPlus.h"

namespace types
{
/*!
 *  \class zintegerT
 *  \brief Our own implementation of std::complex<T> for SIX and friends.
 *
 * `std::complex<TInteger>` is no longer valid C++; provide a (partial) work-around.
 * See https://en.cppreference.com/w/cpp/numeric/complex for details.
 *
 * SIX (and others) mostly use `std::complex<TInt>` as a convenient
 * package for two values; very little "complex math" is done using integers.
 */
template <typename T>
struct zintegerT final
{
    using value_type = T;
    static_assert(!std::is_floating_point<T>::value, "Use std::complex<T> for floating-point.");
    static_assert(std::is_signed<T>::value, "T should be a signed integer.");

    zintegerT(value_type re = 0, value_type im = 0) : z{re, im} { }
    zintegerT(const zintegerT&) = default;
    zintegerT& operator=(const zintegerT&) = default;
    zintegerT(zintegerT&&) = default;
    zintegerT& operator=(zintegerT&&) = default;
    ~zintegerT() = default;

    // If someone already has a std::complex<value_type>, is there any harm in creating ours?
    zintegerT(const std::complex<value_type>& z_) : zintegerT(z_.real(), z_.imag()) { }

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
#pragma warning(disable : 4996)  // '...': warning STL4037: The effect of instantiating the template std::complex for any type other than float, double, or long double is unspecified. You can define _SILENCE_NONFLOATING_COMPLEX_DEPRECATION_WARNING to suppress this warning.
#endif
// Getting different results with GCC vs MSVC :-(  So just use
// std::complex<short> Assume by the time we're actually using C++23 with a
// compiler that enforces this restriction, "something" will be different.
template <typename T>
inline const std::complex<T>& cast(const zintegerT<T>& z)
{
    const void* const pZ_ = &z;
    return *static_cast<const std::complex<T>*>(pZ_);
}
template <typename T>
inline std::complex<T>& cast(zintegerT<T>& z)
{
    void* const pZ_ = &z;
    return *static_cast<std::complex<T>*>(pZ_);
}
CODA_OSS_disable_warning_pop

// https://en.cppreference.com/w/cpp/numeric/complex/operator_ltltgtgt
template <typename T, typename CharT, typename Traits>
inline auto& operator<<(std::basic_ostream<CharT, Traits>& o, const zintegerT<T>& z)
{
    return o << cast(z);
}
template <typename T, typename CharT, typename Traits>
inline auto& operator>>(std::basic_istream<CharT, Traits>& o, zintegerT<T>& z)
{
    return o >> cast(z);
}

// https://en.cppreference.com/w/cpp/numeric/complex/operator_cmp
template <typename T>
inline bool operator==(const zintegerT<T>& lhs, const zintegerT<T>& rhs)
{
    return (lhs.real() == rhs.real()) && (lhs.imag() == rhs.imag());
}
template <typename T>
inline bool operator!=(const zintegerT<T>& lhs, const zintegerT<T>& rhs)
{
    return !(lhs == rhs);
}

// Keep functions like abs() to a minimum; complex math probably shouldn't be done with integers.
template <typename T>
inline auto abs(const zintegerT<T>& z) // https://en.cppreference.com/w/cpp/numeric/complex/abs
{
    return abs(cast(z));
}

// Control whether zinteger is std::complex or details::zintegerT.
// If it is std::complex, then a types::zinteger overload normally can't be
// used as it will be the same as std::complex
#ifdef CODA_OSS_types_FORCE_unique_zinteger // bypass checks below
#define CODA_OSS_types_unique_zinteger 1
#endif
#ifdef CODA_OSS_types_NO_unique_zinteger
#ifdef CODA_OSS_types_unique_zinteger
#error "CODA_OSS_types_unique_zinteger already #define'd"
#endif
#define CODA_OSS_types_unique_zinteger 0
#endif

#ifndef CODA_OSS_types_unique_zinteger
// If the warning about using std::complex<short> has been turned off, we might
// as well use std:complex<short>.
#ifdef _SILENCE_NONFLOATING_COMPLEX_DEPRECATION_WARNING
#define CODA_OSS_types_unique_zinteger 0
#endif
#endif

#ifndef CODA_OSS_types_unique_zinteger
#define CODA_OSS_types_unique_zinteger 1
#endif

template<typename T>
#if CODA_OSS_types_unique_zinteger
using zinteger = zintegerT<T>;
#else
using zinteger = std::complex<T>;
#endif

namespace details
{
// Explicit specializations so that clients can't do zreal<int>
template<typename T> struct zreal;
template<> struct zreal<float> final
{
    using type = std::complex<float>;
};
template<> struct zreal<double> final
{
    using type = std::complex<double>;
};
template<> struct zreal<long double> final
{
    using type = std::complex<long double>;
};
} // namespace details
template<typename T>
using zreal = typename details::zreal<T>::type;

// This might be more trouble than it's worth: there really isn't that much code
// that is generic for both integer and real complex types; recall that the primary
// use of `std::integer<short>` is a "convenient package" for two values.
// 
//Have the compiler pick between std::complex and details::complex
//template<typename T>
//using complex = std::conditional_t<std::is_floating_point<T>::value, zreal_t<T>, zinteger_t<T>>;
static_assert(sizeof(std::complex<short>) == sizeof(zintegerT<short>), "sizeof(sizeof(std::complex<short>) != sizeof(zintegerT<short>)");
static_assert(std::is_same<std::complex<float>, zreal<float>>::value, "should be std::complex<float>");

// Convenient aliases
using zfloat = zreal<float>; // std::complex<float>
using zdouble = zreal<double>; // std::complex<double>
//using zlong_double = zreal_t<long double>; // std::complex<long double>
using zint8_t = zinteger<int8_t>;  // details:complex<int8_t>
using zint16_t = zinteger<int16_t>;  // details:complex<int16_t>
using zint32_t = zinteger<int32_t>;  // details::complex<int32_t>
using zint64_t = zinteger<int64_t>;  // details::complex<int64_t>
}

#endif  // CODA_OSS_types_complex_h_INCLUDED_

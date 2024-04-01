/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * © Copyright 2024, Maxar Technologies, Inc.
 *
 *  six-c++ is free software; you can redistribute it and/or modify
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

#include <stdint.h>
#include <assert.h>

#include <std/string>
#include <utility>
#include <cinttypes>
#include <stdexcept>
#include <limits>
#include <type_traits>

#include <gsl/gsl.h>
#include <config/compiler_extensions.h>

namespace six
{
// A *xs:integer* (and friends such as *xs:positiveInteger*) is unbounded and 
// could be larger than a 64-bit value; *xs:int* and *xs:long* are fixed sizes.
//
// For better or worse, many of the schemas use *xs:integer*; unfortunately,
// existing code mapped this to `int32_t` (i.e., `int`) instead of `int64_t`.
// This class encapsulates these concerns and also makes it possible to hook-up
//  "BigInt" support if that's ever needed.
// 
// See https://www.w3schools.com/xml/schema_dtypes_numeric.asp for details.

// Template parameters allow a single class to support *positiveInterger*, etc.
namespace details
{
    template<bool allowZero, bool allowPositive, bool allowNegative>
    class XsInteger final
    {
        // Since this is XML, it "should" be `std::u8string`, but that's too klunky
        // with existing code.  Furthermore, these are strings representing
        // integers, so the number of valid characters is extremly limited and
        // all part of ASCII.  (Well ... actually, there are whole bunch of "digits"
        // in Unicode, but supporting those here is such an extreme corner case.)
        std::string value_; // store the value from XML so we can always round-trip

    public:
        ~XsInteger() = default;
        XsInteger(const XsInteger&) = default;
        XsInteger& operator=(const XsInteger&) = default;
        XsInteger(XsInteger&&) = default;
        XsInteger& operator=(XsInteger&&) = default;

        explicit XsInteger(std::string v) : value_(std::move(v)) {}
        XsInteger& operator=(std::string v)
        {
            value_ = std::move(v);
            return *this;
        }

        const std::string& str() const noexcept
        {
            return value_;
        }
        const char* c_str() const noexcept
        {
            return value_.c_str();
        }
    };

    // Parse and convert for easy use in arithmetic, these might throw.
    // 
    // Unlike *xs:int*/*xs:long*, none of the C++ "to" functions explicitly
    // state a size, so don't follow that naming convention.
    template<bool allowZero, bool allowPositive, bool allowNegative>
    inline int64_t to_int64(const XsInteger<allowZero, allowPositive, allowNegative>& i)
    {
        static_assert(sizeof(std::intmax_t) == sizeof(int64_t), "intmax_t != int64_t");
        static_assert(sizeof(int64_t) == sizeof(ptrdiff_t), "int64_t != ptrdiff_t");
        // `allowNegative` will be 'false' for positive and non-negative values.
        //static_assert(allowNegative, "'allowNegative' must be `false` for `int64_t`");
        // `allowPositive` will be `false` for negative and non-positive values
        //static_assert(allowPositive, "'allowPositive' must be `true` for `int64_t`");

        // https://en.cppreference.com/w/cpp/string/byte/strtoimax
        constexpr int base = 10;
        auto retval = std::strtoimax(i.c_str(), nullptr /*endptr*/, base);

        // This is dealing with XML, so it doesn't necessarily have to be ultra-fast.
        // A tiny bit of overhead to reduce code duplication and ensure correctness
        // would seem acceptable.
        CODA_OSS_disable_warning_push
        #if _MSC_VER
        #pragma warning(disable: 4127) // conditional expression is constant
        #endif // _MSC_VER

        if (!allowZero && (retval == 0))
        {
            throw std::invalid_argument("Must be not be zero: " + i.str());
        }

        if (!allowPositive && (retval > 0))
        {
            throw std::invalid_argument("Must be a negative (i.e., non-positive) integer: " + i.str());
        }

        if (!allowNegative && (retval < 0))
        {
            throw std::invalid_argument("Must be a positive (i.e., non-negative) integer: " + i.str());
        }

        CODA_OSS_disable_warning_pop

        return retval;
    }
    template<bool allowZero, bool allowPositive, bool allowNegative>
    inline auto to_int32(const XsInteger<allowZero, allowPositive, allowNegative>& i)
    {
        return gsl::narrow<int32_t>(to_int64(i)); // will throw if i > 32-bit value
    }

    // Intentionally not supported; current thinking is that `unsigned` integers
    // are **only** for bit-twiddling, not manipulating as numbers.
    template<bool allowZero, bool allowPositive=true, bool allowNegative=false>
    uint64_t to_uint64(const XsInteger<allowZero, allowPositive, allowNegative>&) = delete;
    template<bool allowZero, bool allowPositive, bool allowNegative>
    uint32_t to_uint32(const XsInteger<allowZero, allowPositive, allowNegative>&) = delete;
}

using XsInteger = details::XsInteger<true /*allowZero*/, true /*allowPositive*/, true /*allowNegative*/>;
inline auto to_int64(const XsInteger& v)
{
    return details::to_int64(v);
}
inline auto toInteger(int64_t v)
{
    XsInteger retval(std::to_string(v));
    assert(to_int64(retval) == v);
    return retval;
}
uint64_t to_uint64(const XsInteger&) = delete;
XsInteger toInteger(uint64_t) = delete;

// These are largely for completeness and illustrative purposes.
// 
// Very little (if any) existing code "validates" at the C++ level; rather, it's all done through the XSD.
// This (mostly) keeps C++ simple (albeit not 100% compliant) and allows two execution paths through the
// code: validating (against the schema) and non-validating.  "Excessive" C++ "validation" may not be desired.
using XsNonNegativeInteger = details::XsInteger<true /*allowZero*/, true /*allowPositive*/, false /*allowNegative*/>;
inline auto to_int64(const XsNonNegativeInteger& v)
{
    return details::to_int64(v);
}
uint64_t to_uint64(const XsNonNegativeInteger&) = delete;
inline auto toNonNegativeInteger(int64_t v)
{
    if (v < 0) // allowZero=true, allowNegative=false
    {
        throw std::invalid_argument("value must be >= 0");
    }
    return XsNonNegativeInteger(toInteger(v).str());
}
XsNonNegativeInteger toNonNegativeInteger(uint64_t) = delete;

using XsPositiveInteger = details::XsInteger<false /*allowZero*/, true /*allowPositive*/, false /*allowNegative*/>;
inline auto to_int64(const XsPositiveInteger& v)
{
    return details::to_int64(v);
}
uint64_t to_uint64(const XsPositiveInteger&) = delete;
inline auto toPositiveInteger(int64_t v)
{
    if (v <= 0) // allowZero=false, allowNegative=false
    {
        throw std::invalid_argument("value must be > 0");
    }
    return XsPositiveInteger(toNonNegativeInteger(v).str());
}
XsPositiveInteger toPositiveInteger(uint64_t) = delete;

using XsNonPositiveInteger = details::XsInteger<true /*allowZero*/, false /*allowPositive*/, true /*allowNegative*/>;
inline auto to_int64(const XsNonPositiveInteger& v)
{
    return details::to_int64(v);
}
inline auto toNonPositiveInteger(int64_t v)
{
    if (v > 0) // allowZero=true, allowNegative=false
    {
        throw std::invalid_argument("value must be <= 0");
    }
    return XsNonPositiveInteger(toInteger(v).str());
}
// Only one valid valid for `uint64_t`: 0; all others are positive, i.e., not non-positive
uint64_t to_uint64(const XsNonPositiveInteger&) = delete;
XsNonPositiveInteger toNonPositiveInteger(uint64_t) = delete;

using XsNegativeInteger = details::XsInteger<false /*allowZero*/, false /*allowPositive*/, true /*allowNegative*/>;
inline auto to_int64(const XsNegativeInteger& v)
{
    return details::to_int64(v);
}
inline auto toNegativeInteger(int64_t v)
{
    if (v >= 0) // allowZero=false, allowNegative=false
    {
        throw std::invalid_argument("value must be < 0");
    }
    return XsNegativeInteger(toInteger(v).str());
}
// All values for `uint64_t` are >=0
uint64_t to_uint64(const XsNegativeInteger&) = delete;
XsNegativeInteger toNegativeInteger(uint64_t) = delete;
}


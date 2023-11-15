/* =========================================================================
 * This file is part of str-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * str-c++ is free software; you can redistribute it and/or modify
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
#ifndef CODA_OSS_str_Convert_h_INCLUDED_
#define CODA_OSS_str_Convert_h_INCLUDED_

#include <cerrno>
#include <complex>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <ostream>
#include <sstream>
#include <string>
#include <typeinfo>

#include "config/Exports.h"
#include "coda_oss/string.h"
#include "coda_oss/optional.h"
#include "coda_oss/cstddef.h"
#include "types/Complex.h"
#include "import/except.h"
#include "gsl/gsl.h"
#include "str/Encoding.h"

namespace str
{
template <typename T> int getPrecision(const T& type);
template <typename T> int getPrecision(const std::complex<T>&);
#if CODA_OSS_types_unique_ComplexInteger
template <typename T> int getPrecision(const types::ComplexInteger<T>&);
#endif

// Note that std::to_string() doesn't necessarily generate the same output as writing
// to std::cout; see https://en.cppreference.com/w/cpp/string/basic_string/to_string
template <typename T>
std::string toString_(const T& value)
{
    std::ostringstream buf;
    buf.precision(getPrecision(value));
    buf << std::boolalpha << value;
    return buf.str();
}
template <typename T>
inline std::string toString(const T& value)
{
    return toString_(value);
}

// https://en.cppreference.com/w/cpp/string/basic_string/to_string
inline auto toString(int value)
{
    return toString_(value);
}
inline auto toString(long value)
{
    return toString_(value);
}
inline auto toString(long long value)
{
    return toString_(value);
}
inline auto toString(unsigned value)
{
    return toString_(value);
}
inline auto toString(unsigned long value)
{
    return toString_(value);
}
inline auto toString(unsigned long long value)
{
    return toString_(value);
}
inline auto toString(float value)
{
    return toString_(value);
}
inline auto toString(double value)
{
    return toString_(value);
}
inline auto toString(long double value)
{
    return toString_(value);
}

// https://en.cppreference.com/w/cpp/string/basic_string/to_string
inline auto toString(int value)
{
    return toString_(value);
}
inline auto toString(long value)
{
    return toString_(value);
}
inline auto toString(long long value)
{
    return toString_(value);
}
inline auto toString(unsigned value)
{
    return toString_(value);
}
inline auto toString(unsigned long value)
{
    return toString_(value);
}
inline auto toString(unsigned long long value)
{
    return toString_(value);
}
inline auto toString(float value)
{
    return toString_(value);
}
inline auto toString(double value)
{
    return toString_(value);
}
inline auto toString(long double value)
{
    return toString_(value);
}
inline std::string toString(uint8_t value)
{
    return toString(gsl::narrow<unsigned int>(value));
}
inline std::string toString(int8_t value)
{
    return toString(gsl::narrow<int>(value));
}
inline std::string toString(coda_oss::byte value)
{
    return toString(gsl::narrow<uint8_t>(value));
}

inline std::string toString(std::nullptr_t)
{
    return "<nullptr>";
}

inline std::string toString(const std::string& value)
{
    return value;
}
// Prevent the template above from getting used; instead, use routines from **Encoding.h**.
std::string toString(const std::wstring&) = delete;
std::string toString(const std::u16string&) = delete;
std::string toString(const std::u32string&) = delete;
std::string toString(const coda_oss::u8string&) = delete;
std::string toString(const str::W1252string&) = delete;

inline std::string toString(std::string::const_pointer pStr)
{
    return toString(std::string(pStr));
}
// can't be a template; `bool` overload above is a better match
std::string toString(std::wstring::const_pointer) = delete; // only used in unittests
std::string toString(std::u16string::const_pointer) = delete; // only used in unittests
std::string toString(std::u32string::const_pointer) = delete; // only used in unittests

inline std::ostream& operator<<(std::ostream& os, const coda_oss::u8string& s)
{
    os << to_native(s);
    return os;
}

inline std::string toString(char value)
{
    return std::string(1, value);
}

template <typename T>
inline std::string toString(const coda_oss::optional<T>& value)
{
    // TODO: handle empty/NULL optional?
    return toString(value.value());
}

template<typename T>
inline std::string toString(const T& real, const T& imag)
{
    return toString(std::complex<T>(real, imag));
}

template <typename T>
T toType(const std::string& s)
{
    if (s.empty())
        throw except::BadCastException(
                except::Context(__FILE__,
                                __LINE__,
                                std::string(""),
                                std::string(""),
                                std::string("Empty string")));

    T value;

    std::stringstream buf(s);
    buf.precision(str::getPrecision(value));
    buf >> value;

    if (buf.fail())
    {
        throw except::BadCastException(
                except::Context(__FILE__,
                                __LINE__,
                                std::string(""),
                                std::string(""),
                                std::string("Conversion failed: '") + s +
                                        std::string("' -> ") +
                                        typeid(T).name()));
    }

    return value;
}

template <>
CODA_OSS_API bool toType<bool>(const std::string& s);
template <>
CODA_OSS_API std::string toType<std::string>(const std::string& s);

/**
 *  strtoll wrapper for msvc compatibility.
 */
CODA_OSS_API long long strtoll(const char* str, char** endptr, int base);
/**
 *  strtoull wrapper for msvc compatibility.
 */
CODA_OSS_API unsigned long long strtoull(const char* str, char** endptr, int base);

/**
 *  Convert a string containing a number in any base to a numerical type.
 *
 *  @param s a string containing a number in base base
 *  @param base the base of the number in s
 *  @return a numberical representation of the number
 *  @throw BadCastException thrown if cast cannot be performed.
 */
template <typename T>
T toType(const std::string& s, int base)
{
    char* end;
    errno = 0;
    const char* str = s.c_str();

    T res;
    bool overflow = false;
    if (std::numeric_limits<T>::is_signed)
    {
        const long long longRes = str::strtoll(str, &end, base);
        if (longRes < static_cast<long long>(std::numeric_limits<T>::min()) ||
            longRes > static_cast<long long>(std::numeric_limits<T>::max()))
        {
            overflow = true;
        }
        res = static_cast<T>(longRes);
    }
    else
    {
        const unsigned long long longRes = str::strtoull(str, &end, base);
        if (longRes < static_cast<unsigned long long>(
                              std::numeric_limits<T>::min()) ||
            longRes > static_cast<unsigned long long>(
                              std::numeric_limits<T>::max()))
        {
            overflow = true;
        }
        res = static_cast<T>(longRes);
    }

    if (overflow || errno == ERANGE)
        throw except::BadCastException(
                except::Context(__FILE__,
                                __LINE__,
                                std::string(""),
                                std::string(""),
                                std::string("Overflow: '") + s +
                                        std::string("' -> ") +
                                        typeid(T).name()));
    // If the end pointer is at the start of the string, we didn't convert
    // anything.
    else if (end == str)
        throw except::BadCastException(
                except::Context(__FILE__,
                                __LINE__,
                                std::string(""),
                                std::string(""),
                                std::string("Conversion failed: '") + s +
                                        std::string("' -> ") +
                                        typeid(T).name()));

    return res;
}

/**
 *  Determine the precision required for the data type.
 *
 *  @param type A variable of the type whose precision argument is desired.
 *  @return The integer argument required by ios::precision() to represent
 *  this type.
 */
template <typename T>
int getPrecision(const T&)
{
    return 0;
}

template <typename T>
int getPrecision(const std::complex<T>& type)
{
    return getPrecision(type.real());
}
#if CODA_OSS_types_unique_ComplexInteger
template <typename T>
int getPrecision(const types::ComplexInteger<T>& type)
{
    return getPrecision(type.real());
}
#endif

template <>
int getPrecision(const float& type);

template <>
int getPrecision(const double& type);

template <>
int getPrecision(const long double& type);

/** Generic casting routine; used by explicitly overloaded
 conversion operators.

 @param value A variable of the type being cast to.
 @return The internal representation of GenericType, converted
 to the desired type, if possible.
 @throw BadCastException thrown if cast cannot be performed.
 */
template <typename T>
T generic_cast(const std::string& value)
{
    return str::toType<T>(value);
}

}

#endif // CODA_OSS_str_Convert_h_INCLUDED_

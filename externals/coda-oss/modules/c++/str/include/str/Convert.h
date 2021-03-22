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

#ifndef __STR_CONVERT_H__
#define __STR_CONVERT_H__
#pragma once

#include <import/except.h>
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

// This is a fairly low-level file, so don't #include a lot of our other files
#include "str/String_.h"

namespace str
{
template <typename T>
int getPrecision(const T& type);

template <typename T>
int getPrecision(const std::complex<T>& type);

template <typename T>
std::string toString(const T& value)
{
    std::ostringstream buf;
    buf.precision(getPrecision(value));
    buf << std::boolalpha << value;
    return buf.str();
}

template <>
std::string toString(const uint8_t& value);

template <>
std::string toString(const int8_t& value);

template <>
inline std::string toString(const std::nullptr_t&)
{
    return "<nullptr>";
}

template <typename T>
std::string toString(const T& real, const T& imag)
{
    return toString(std::complex<T>(real, imag));
}

template <>
inline std::string toString(const sys::U8string& value)
{
    // This is OK as UTF-8 can be stored in std::string
    // Note that casting between the string types will CRASH on some implementatons.
    // NO: reinterpret_cast<const std::string&>(value)
    const void* const pValue = value.c_str();
    return static_cast<std::string::const_pointer>(pValue);  // copy
}

inline sys::U8string castToU8string(const std::string& value)
{
    // This is dangerous as we don't know the encoding of std::string!
    // If it is Windows-1252, the reteurned sys::U8string will be garbage.
    // Only use when you are sure of the encoding.
    const void* const pValue = value.c_str();
    return static_cast<sys::U8string::const_pointer>(pValue);
}

sys::U8string fromWindows1252(const std::string&);
void fromWindows1252(const std::string&, sys::U8string&);
void fromWindows1252(const std::string&, std::string&);

sys::U8string toUtf8(const std::u16string&);
sys::U8string toUtf8(const std::u32string&);

void toUtf8(const std::u16string&, sys::U8string&);
void toUtf8(const std::u16string&, std::string&);
void toUtf8(const std::u32string&, sys::U8string&);
void toUtf8(const std::u32string&, std::string&);

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
bool toType<bool>(const std::string& s);
template <>
std::string toType<std::string>(const std::string& s);

/**
 *  strtoll wrapper for msvc compatibility.
 */
long long strtoll(const char* str, char** endptr, int base);
/**
 *  strtoull wrapper for msvc compatibility.
 */
unsigned long long strtoull(const char* str, char** endptr, int base);

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

#endif

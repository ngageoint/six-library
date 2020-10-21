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

#include "str/Convert.h"
#include "str/Manip.h"

template<> std::string str::toType<std::string>(const std::string& s)
{
    return s;
}

template<> std::string str::toString(const uint8_t& value)
{
    return str::toString(static_cast<unsigned int>(value));
}

template<> std::string str::toString(const int8_t& value)
{
    return str::toString(static_cast<int>(value));
}

template<> bool str::toType<bool>(const std::string& s)
{
    std::string ss = s;
    str::lower(ss);

    if (ss == "true")
    {
        return true;
    }
    else if (ss == "false")
    {
        return false;
    }
    else if (str::isNumeric(ss))
    {
        int value(0);
        std::stringstream buf(ss);
        buf >> value;
        return (value != 0);
    }
    else
    {
        throw except::BadCastException(except::Context(__FILE__, __LINE__,
            std::string(""), std::string(""),
            std::string("Invalid bool: '") + s + std::string("'")));
    }

    return false;
}

long long str::strtoll(const char *str, char **endptr, int base)
{
#if defined(_MSC_VER)
    return _strtoi64(str, endptr, base);
#else
    return ::strtoll(str, endptr, base);
#endif
}

unsigned long long str::strtoull(const char *str, char **endptr, int base)
{
#if defined(_MSC_VER)
    return _strtoui64(str, endptr, base);
#else
    return ::strtoull(str, endptr, base);
#endif
}

template<> int str::getPrecision(const float& )
{
    return std::numeric_limits<float>::max_digits10;
}
template<> int str::getPrecision(const double& )
{
    return std::numeric_limits<double>::max_digits10;
}
template<> int str::getPrecision(const long double& )
{
    return std::numeric_limits<long double>::max_digits10;
}

// Convert a single ISO8859-1 character to UTF-8
// https://en.wikipedia.org/wiki/ISO/IEC_8859-1
static inline sys::u8string::value_type cast(std::string::value_type ch)
{
    static_assert(sizeof(sys::u8string::value_type) == sizeof(std::string::value_type),
        "sizeof(Char8_T) != sizeof(char)");
    return static_cast<sys::u8string::value_type>(ch);
}
static sys::u8string to_utf8(std::string::value_type ch)
{
    if ((ch >= '\x00') && (ch <= '\x7f'))  // ASCII
    {
        return sys::u8string{cast(ch)};
    }

    if ((ch >= '\xC0' /*À*/) && (ch <= '\xFF' /*y*/))  // ISO8859-1 letters
    {
        sys::u8string retval{cast('\xC3')};
        ch -= 0x40;  // 0xC0 -> 0x80
        retval.push_back(cast(ch));
        return retval;
    }

    return sys::u8string{cast(ch)};  // ???
}
sys::u8string str::toUtf8(const std::string& str)
{
    sys::u8string retval;
    // Assume the input string is ISO8859-1 (western European) and convert to UTF-8
    for (const auto& ch : str)
    {
        retval += to_utf8(ch);
    }
    return retval;
}

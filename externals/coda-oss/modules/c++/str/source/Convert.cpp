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

#include <assert.h>

#ifdef _WIN32
#include <codecvt> // not available in older versions of GCC, even with --std=c++11
#endif
#include <map>

#include "str/Convert.h"
#include "str/Manip.h"
#include "str/utf8.h"

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

static sys::u8string utf8_(uint32_t ch)
{
    const std::u32string s{static_cast<std::u32string::value_type>(ch)};
    return str::toUtf8(s);
}
#define utf8(ch) utf8_(ch)

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

    // These characters can be converted from ISO8859-1 to UTF-8 with simple math
    // See http://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WINDOWS/CP1252.TXT
    if ((ch >= '\xA0' /*NO-BREAK SPACE*/) && (ch <= '\xFF' /*LATIN SMALL LETTER Y WITH DIAERESIS*/))
    {
        ch -= 0x40;  // 0xC0 -> 0x80
        return sys::u8string {cast('\xC3'), cast(ch)};
    }

    // Need to look up characters from \x80 (EURO SIGN) to \x9F (LATIN CAPITAL LETTER Y WITH DIAERESIS)
    // in a map (see above).
    static const std::map<std::string::value_type, sys::u8string> x80_x9F_to_u8string
    {
        {'\x80', utf8(0x20AC) } // EURO SIGN
        // UNDEFINED
        , {'\x82', utf8(0x201A) } // SINGLE LOW-9 QUOTATION MARK
        , {'\x83', utf8(0x0192)  } // LATIN SMALL LETTER F WITH HOOK
        , {'\x84', utf8(0x201E)  } // DOUBLE LOW-9 QUOTATION MARK
        , {'\x85', utf8(0x2026)  } // HORIZONTAL ELLIPSIS
        , {'\x86', utf8(0x2020)  } // DAGGER
        , {'\x87', utf8(0x2021)  } // DOUBLE DAGGER
        , {'\x88', utf8(0x02C6)  } // MODIFIER LETTER CIRCUMFLEX ACCENT
        , {'\x89', utf8(0x2030)  } // PER MILLE SIGN
        , {'\x8A', utf8(0x0160)  } // LATIN CAPITAL LETTER S WITH CARON
        , {'\x8B', utf8(0x2039)  } // SINGLE LEFT-POINTING ANGLE QUOTATION MARK
        , {'\x8C', utf8(0x0152)  } // LATIN CAPITAL LIGATURE OE
        // UNDEFINED
        , {'\x8E', utf8(0x017D)  } // LATIN CAPITAL LETTER Z WITH CARON
        // UNDEFINED
        // UNDEFINED
        , {'\x91', utf8(0x017D)  } // LEFT SINGLE QUOTATION MARK
        , {'\x92', utf8(0x2018)  } // RIGHT SINGLE QUOTATION MARK
        , {'\x93', utf8(0x2019)  } // LEFT DOUBLE QUOTATION MARK
        , {'\x94', utf8(0x201D)  } // RIGHT DOUBLE QUOTATION MARK
        , {'\x95', utf8(0x2022)  } // BULLET
        , {'\x96', utf8(0x2013)  } // EN DASH
        , {'\x97', utf8(0x2014)  } // EM DASH
        , {'\x98', utf8(0x02DC)  } // SMALL TILDE
        , {'\x99', utf8(0x2122)  } // TRADE MARK SIGN
        , {'\x9A', utf8(0x0161)  } // LATIN SMALL LETTER S WITH CARON
        , {'\x9B', utf8(0x203A)  } // SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
        , {'\x9C', utf8(0x0153)  } // LATIN SMALL LIGATURE OE
        // UNDEFINED
        , {'\x9E', utf8(0x017E)  } // LATIN SMALL LETTER Z WITH CARON
        , {'\x9F', utf8(0x0178)  } // LATIN CAPITAL LETTER Y WITH DIAERESIS
    };
    const auto it = x80_x9F_to_u8string.find(ch);
    if (it != x80_x9F_to_u8string.end())
    {
        return it->second;
    }

    // If we make it here, the input text contains a character that isn't defined in
    // Windows-1252; return a "replacment character."  Yes, this will **corrupt**
    // the input data as information is lost:
    // https://en.wikipedia.org/wiki/Specials_(Unicode_block)#Replacement_character
    static const sys::u8string replacement_character = utf8(0xfffd);
    return replacement_character;
}
#undef utf8
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

#ifdef _WIN32 // <codecvt> might not be available with GCC
// https://en.cppreference.com/w/cpp/locale/codecvt_utf8
template <typename T>
static void codecvt_toUtf8_(const T& str, std::string& result)
{
    // Note this is all depreicated in C++17 ... but there is no standard replacement.

    // https:en.cppreference.com/w/cpp/locale/codecvt
    using value_type = typename T::value_type;
    std::wstring_convert<std::codecvt_utf8<value_type>, value_type> conv;

    // https://en.cppreference.com/w/cpp/locale/wstring_convert/to_bytes
    result = conv.to_bytes(str);
}
static void codecvt_toUtf8(const std::u16string& str, std::string& result)
{
    return codecvt_toUtf8_(str, result);
}
static void codecvt_toUtf8(const std::u32string& str, std::string& result)
{
    return codecvt_toUtf8_(str, result);
}
#endif

void str::toUtf8(const std::u16string& str, std::string& result)
{
    // http://utfcpp.sourceforge.net/#introsample
    /*
    // Convert it to utf-16
    vector<unsigned short> utf16line;
    utf8::utf8to16(line.begin(), end_it, back_inserter(utf16line));

    // And back to utf-8
    string utf8line;
    utf8::utf16to8(utf16line.begin(), utf16line.end(), back_inserter(utf8line));
    */
    utf8::utf16to8(str.begin(), str.end(), back_inserter(result));

    #if defined(_WIN32) && defined(_DEBUG) && !defined(NDEBUG) // check utf8:: routines
    std::string codecvt_result;
    codecvt_toUtf8(str, codecvt_result);
    assert(codecvt_result == result);
    #endif
}
void str::toUtf8(const std::u32string& str, std::string& result)
{
    utf8::utf32to8(str.begin(), str.end(), back_inserter(result));
    #if defined(_WIN32) && defined(_DEBUG) && !defined(NDEBUG) // check utf8:: routines
    std::string codecvt_result;
    codecvt_toUtf8(str, codecvt_result);
    assert(codecvt_result == result);
    #endif
}

template<typename T>
static sys::u8string toUtf8(const T& str)
{
    // trying to avoid a copy by casting between std::string/sys::u8string
    // causes a nasty crash on some platforms
    std::string utf8;
    str::toUtf8(str, utf8);
    return reinterpret_cast<const sys::u8string::value_type*>(utf8.c_str()); // copy here
}
sys::u8string str::toUtf8(const std::u16string& str)
{
    return ::toUtf8(str);
}
sys::u8string str::toUtf8(const std::u32string& str)
{
    return ::toUtf8(str);
}

void str::toUtf8(const std::u16string& str, sys::u8string& result)
{
    result = toUtf8(str);
}
void str::toUtf8(const std::u32string& str, sys::u8string& result)
{
    result = toUtf8(str);
}

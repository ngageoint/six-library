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
#include <string.h> // strlen()

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

// Convert a single Windows-1252 character to UTF-8
// https://en.wikipedia.org/wiki/ISO/IEC_8859-1
static constexpr sys::u8string::value_type cast(uint8_t ch)
{
    static_assert(sizeof(decltype(ch)) == sizeof(sys::u8string::value_type), "sizeof(uint8_t) != sizeof(Char8_t)");
    return static_cast<sys::u8string::value_type>(ch);
}
static sys::u8string fromWindows1252(std::string::value_type ch_)
{
    const auto ch = static_cast<uint8_t>(ch_);

    // Need to look up characters from \x80 (EURO SIGN) to \x9F (LATIN CAPITAL LETTER Y WITH DIAERESIS)
    // in a map: http://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WINDOWS/CP1252.TXT
    //
    // If the input text contains a character that isn't defined in Windows-1252;
    // return a "replacment character."  Yes, this will **corrupt** the input data as information is lost:
    // https://en.wikipedia.org/wiki/Specials_(Unicode_block)#Replacement_character
    static const auto utf8 = [](uint32_t ch) {
        const std::u32string s{static_cast<std::u32string::value_type>(ch)};
        return str::toUtf8(s);
    };
    static const sys::u8string replacement_character = utf8(0xfffd);
    static const std::map<uint32_t, sys::u8string> x80_x9F_to_u8string
    {
        {0x80, utf8(0x20AC) } // EURO SIGN
        , {0x81, replacement_character } // UNDEFINED
        , {0x82, utf8(0x201A) } // SINGLE LOW-9 QUOTATION MARK
        , {0x83, utf8(0x0192)  } // LATIN SMALL LETTER F WITH HOOK
        , {0x84, utf8(0x201E)  } // DOUBLE LOW-9 QUOTATION MARK
        , {0x85, utf8(0x2026)  } // HORIZONTAL ELLIPSIS
        , {0x86, utf8(0x2020)  } // DAGGER
        , {0x87, utf8(0x2021)  } // DOUBLE DAGGER
        , {0x88, utf8(0x02C6)  } // MODIFIER LETTER CIRCUMFLEX ACCENT
        , {0x89, utf8(0x2030)  } // PER MILLE SIGN
        , {0x8A, utf8(0x0160)  } // LATIN CAPITAL LETTER S WITH CARON
        , {0x8B, utf8(0x2039)  } // SINGLE LEFT-POINTING ANGLE QUOTATION MARK
        , {0x8C, utf8(0x0152)  } // LATIN CAPITAL LIGATURE OE
        , {0x8D, replacement_character } // UNDEFINED
        , {0x8E, utf8(0x017D)  } // LATIN CAPITAL LETTER Z WITH CARON
        , {0x8F, replacement_character } // UNDEFINED
        , {0x90, replacement_character } // UNDEFINED
        , {0x91, utf8(0x017D)  } // LEFT SINGLE QUOTATION MARK
        , {0x92, utf8(0x2018)  } // RIGHT SINGLE QUOTATION MARK
        , {0x93, utf8(0x2019)  } // LEFT DOUBLE QUOTATION MARK
        , {0x94, utf8(0x201D)  } // RIGHT DOUBLE QUOTATION MARK
        , {0x95, utf8(0x2022)  } // BULLET
        , {0x96, utf8(0x2013)  } // EN DASH
        , {0x97, utf8(0x2014)  } // EM DASH
        , {0x98, utf8(0x02DC)  } // SMALL TILDE
        , {0x99, utf8(0x2122)  } // TRADE MARK SIGN
        , {0x9A, utf8(0x0161)  } // LATIN SMALL LETTER S WITH CARON
        , {0x9B, utf8(0x203A)  } // SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
        , {0x9C, utf8(0x0153)  } // LATIN SMALL LIGATURE OE
        , {0x9D, replacement_character } // UNDEFINED
        , {0x9E, utf8(0x017E)  } // LATIN SMALL LETTER Z WITH CARON
        , {0x9F, utf8(0x0178)  } // LATIN CAPITAL LETTER Y WITH DIAERESIS
    };
    const auto it = x80_x9F_to_u8string.find(ch);
    if (it != x80_x9F_to_u8string.end())
    {
        return it->second;
    }

    // If it's not in the map, convert to UTF-8
    // https://stackoverflow.com/questions/4059775/convert-iso-8859-1-strings-to-utf-8-in-c-c
    assert((ch < 0x80) || (ch > 0x9F));
    if (ch < 0x80)
    {
        return sys::u8string{cast(ch)}; // ASCII
    }
    // *out++=0xc2+(*in>0xbf), *out++=(*in++&0x3f)+0x80;
    return sys::u8string{cast(0xc2 + (ch > 0xbf)), cast((ch & 0x3f) + 0x80)}; // ISO8859-1
}
sys::u8string str::fromWindows1252(const std::string& str)
{
    sys::u8string retval;
    // Assume the input string is Windows-1252 (western European) and convert to UTF-8
    for (const auto& ch : str)
    {
        retval += ::fromWindows1252(ch);
    }
    return retval;
}

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
    utf8::utf16to8(str.begin(), str.end(), std::back_inserter(result));
}
void str::toUtf8(const std::u32string& str, std::string& result)
{
    utf8::utf32to8(str.begin(), str.end(), std::back_inserter(result));
}

struct back_inserter final
{ 
    sys::u8string* container = nullptr;
    explicit back_inserter(sys::u8string& s) noexcept : container(&s) { }

    back_inserter& operator=(uint8_t v)
    {
        container->push_back(static_cast<sys::u8string::value_type>(v));
        return *this;
    }
    back_inserter& operator*() noexcept { return *this; }
    back_inserter operator++(int) noexcept { return *this; }
};
sys::u8string str::toUtf8(const std::u16string& str)
{
    sys::u8string retval;
    utf8::utf16to8(str.begin(), str.end(), back_inserter(retval));
    return retval;
}
sys::u8string str::toUtf8(const std::u32string& str)
{
    sys::u8string retval;
    utf8::utf32to8(str.begin(), str.end(), back_inserter(retval));
    return retval;
}

void str::toUtf8(const std::u16string& str, sys::u8string& result)
{
    result = toUtf8(str);
}
void str::toUtf8(const std::u32string& str, sys::u8string& result)
{
    result = toUtf8(str);
}

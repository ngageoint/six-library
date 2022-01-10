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
#include <comdef.h>  // _bstr_t
#endif

#include <map>
#include <locale>
#include <stdexcept>
#include <vector>
#include<iterator>

#include "str/Encoding.h"
#include "str/Manip.h"
#include "str/Convert.h"
#include "str/utf8.h"


// Need to look up characters from \x80 (EURO SIGN) to \x9F (LATIN CAPITAL LETTER Y WITH DIAERESIS)
// in a map: http://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WINDOWS/CP1252.TXT
static inline str::U8string utf8_(uint32_t ch)
{
    const std::u32string s{static_cast<std::u32string::value_type>(ch)};
    str::U8string retval;
    str::utf32to8(s, retval);
    return retval;
};
static const std::map<uint32_t, sys::U8string> Windows1252_x80_x9F_to_u8string{
    {0x80, utf8_(0x20AC) } // EURO SIGN
    // , {0x81, replacement_character } // UNDEFINED
    , {0x82, utf8_(0x201A) } // SINGLE LOW-9 QUOTATION MARK
    , {0x83, utf8_(0x0192)  } // LATIN SMALL LETTER F WITH HOOK
    , {0x84, utf8_(0x201E)  } // DOUBLE LOW-9 QUOTATION MARK
    , {0x85, utf8_(0x2026)  } // HORIZONTAL ELLIPSIS
    , {0x86, utf8_(0x2020)  } // DAGGER
    , {0x87, utf8_(0x2021)  } // DOUBLE DAGGER
    , {0x88, utf8_(0x02C6)  } // MODIFIER LETTER CIRCUMFLEX ACCENT
    , {0x89, utf8_(0x2030)  } // PER MILLE SIGN
    , {0x8A, utf8_(0x0160)  } // LATIN CAPITAL LETTER S WITH CARON
    , {0x8B, utf8_(0x2039)  } // SINGLE LEFT-POINTING ANGLE QUOTATION MARK
    , {0x8C, utf8_(0x0152)  } // LATIN CAPITAL LIGATURE OE
    //, {0x8D, replacement_character } // UNDEFINED
    , {0x8E, utf8_(0x017D)  } // LATIN CAPITAL LETTER Z WITH CARON
    //, {0x8F, replacement_character } // UNDEFINED
    //, {0x90, replacement_character } // UNDEFINED
    , {0x91, utf8_(0x017D)  } // LEFT SINGLE QUOTATION MARK
    , {0x92, utf8_(0x2018)  } // RIGHT SINGLE QUOTATION MARK
    , {0x93, utf8_(0x2019)  } // LEFT DOUBLE QUOTATION MARK
    , {0x94, utf8_(0x201D)  } // RIGHT DOUBLE QUOTATION MARK
    , {0x95, utf8_(0x2022)  } // BULLET
    , {0x96, utf8_(0x2013)  } // EN DASH
    , {0x97, utf8_(0x2014)  } // EM DASH
    , {0x98, utf8_(0x02DC)  } // SMALL TILDE
    , {0x99, utf8_(0x2122)  } // TRADE MARK SIGN
    , {0x9A, utf8_(0x0161)  } // LATIN SMALL LETTER S WITH CARON
    , {0x9B, utf8_(0x203A)  } // SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
    , {0x9C, utf8_(0x0153)  } // LATIN SMALL LIGATURE OE
    //, {0x9D, replacement_character } // UNDEFINED
    , {0x9E, utf8_(0x017E)  } // LATIN SMALL LETTER Z WITH CARON
    , {0x9F, utf8_(0x0178)  } // LATIN CAPITAL LETTER Y WITH DIAERESIS
};

// Convert a single Windows-1252 character to UTF-8
// https://en.wikipedia.org/wiki/ISO/IEC_8859-1
static constexpr sys::U8string::value_type cast(uint8_t ch)
{
    static_assert(sizeof(decltype(ch)) == sizeof(sys::U8string::value_type), "sizeof(uint8_t) != sizeof(Char8_t)");
    return static_cast<sys::U8string::value_type>(ch);
}
static sys::U8string fromWindows1252(uint8_t ch)
{
    // ASCII is the same in UTF-8
    if (ch < 0x80)
    {
        return sys::U8string{cast(ch)};  // ASCII
    }

    // ISO8859-1 can be converted to UTF-8 with bit-twiddling
    if (ch > 0x9F)
    {
        // https://stackoverflow.com/questions/4059775/convert-iso-8859-1-strings-to-utf-8-in-c-c
        // *out++=0xc2+(*in>0xbf), *out++=(*in++&0x3f)+0x80;
        return sys::U8string{cast(0xc2 + (ch > 0xbf)), cast((ch & 0x3f) + 0x80)}; // ISO8859-1
    }

    static const auto map = Windows1252_x80_x9F_to_u8string;
    const auto it = map.find(ch);
    if (it != map.end())
    {
        return it->second;
    }

    // If the input text contains a character that isn't defined in Windows-1252;
    // return a "replacement character."  Yes, this will **corrupt** the input data as information is lost:
    // https://en.wikipedia.org/wiki/Specials_(Unicode_block)#Replacement_character
    static const sys::U8string replacement_character = utf8_(0xfffd);
    return replacement_character;
}
inline sys::U8string fromWindows1252(std::string::value_type ch)
{
    return fromWindows1252(static_cast<uint8_t>(ch));
}
inline sys::U8string fromWindows1252(str::W1252string::value_type ch)
{
    return fromWindows1252(static_cast<std::string::value_type>(ch));
}

void str::windows1252to8(W1252string::const_pointer p, size_t sz, sys::U8string& result)
{
    for (size_t i = 0; i < sz; i++)
    {
        result += ::fromWindows1252(p[i]);    
    }
}

struct back_inserter final
{ 
    sys::U8string* container = nullptr; // pointer instead of reference for copy
    explicit back_inserter(sys::U8string& s) noexcept : container(&s) { }

    back_inserter& operator=(uint8_t v)
    {
        container->push_back(static_cast<sys::U8string::value_type>(v));
        return *this;
    }
    back_inserter& operator*() noexcept { return *this; }
    back_inserter operator++(int) noexcept { return *this; }
};
void str::utf16to8(std::u16string::const_pointer p, size_t sz, sys::U8string& result)
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
    utf8::utf16to8(p, p + sz, back_inserter(result));
    
    /*
    std::vector<unsigned short> utf16line;
    auto begin = c_str<const uint8_t*>(result);
    utf8::utf8to16(begin, begin+result.size(), std::back_inserter(utf16line));
    */
}
void str::utf32to8(std::u32string::const_pointer p, size_t sz, sys::U8string& result)
{
    utf8::utf32to8(p, p + sz, back_inserter(result));
}

void str::utf8to16(sys::U8string::const_pointer p, size_t sz, std::u16string& result)
{
    auto p8 = cast<const uint8_t*>(p);
    utf8::utf8to16(p8, p8 + sz, std::back_inserter(result));
}
void str::utf8to32(sys::U8string::const_pointer p, size_t sz, std::u32string& result)
{
    auto p8 = cast<const uint8_t*>(p);
    utf8::utf8to32(p8, p8 + sz, std::back_inserter(result));
}

inline void wsto8_(std::u16string::const_pointer begin, std::u16string::const_pointer end, sys::U8string& result)
{
    utf8::utf16to8(begin, end, back_inserter(result));
}
inline void wsto8_(std::u32string::const_pointer begin, std::u32string::const_pointer end, sys::U8string& result)
{
    utf8::utf32to8(begin, end, back_inserter(result));
}

sys::U8string str::to_u8string(W1252string::const_pointer p, size_t sz)
{
    sys::U8string retval;
    windows1252to8(p, sz, retval);
    return retval;
}
sys::U8string str::to_u8string(std::string::const_pointer p, size_t sz)
{
    auto platform = details::Platform;  // "conditional expression is constant"
    if (platform == details::PlatformType::Windows)
    {    
        sys::U8string retval;
        windows1252to8(cast<W1252string::const_pointer>(p), sz, retval);
        return retval;
    }
    else if (platform == details::PlatformType::Linux)
    {
        return cast<sys::U8string::const_pointer>(p);  // copy
    }
    throw std::logic_error("Unknown platform.");
}

sys::U8string str::fromWindows1252(std::string::const_pointer p, size_t sz)
{
    return to_u8string(cast<W1252string::const_pointer>(p), sz);
}

sys::U8string str::fromUtf8(std::string::const_pointer p, size_t)
{
    return cast<sys::U8string::const_pointer>(p); // copy
}

template <>
std::string str::toString(const str::U8string& utf8)
{
    auto platform = details::Platform;  // "conditional expression is constant"
    if (platform == details::PlatformType::Windows)
    {
        #ifdef _WIN32
        std::u16string utf16;
        utf8to16(utf8, utf16);
        const _bstr_t s(c_str<std::wstring::const_pointer>(utf16)); // wchar_t is UTF-16 on Windows
        return static_cast<std::string::const_pointer>(s);
        #endif
    }
    else if (platform == details::PlatformType::Linux)
    {
        return c_str<std::string::const_pointer>(utf8);  // copy
    }
    
    throw std::logic_error("Unknown platform.");
}
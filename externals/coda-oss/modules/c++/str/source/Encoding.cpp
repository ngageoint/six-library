/* =========================================================================
 * This file is part of str-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2020, 2021, 2022, Maxar Technologies, Inc.
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
#include <string.h>

#include <map>
#include <locale>
#include <stdexcept>
#include <vector>
#include <iterator>

#include "str/Encoding.h"
#include "str/Manip.h"
#include "str/Convert.h"
#include "str/utf8.h"

// Need to look up characters from \x80 (EURO SIGN) to \x9F (LATIN CAPITAL LETTER Y WITH DIAERESIS)
// in a map: http://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WINDOWS/CP1252.TXT
inline void utf32to8(const std::u32string& s, sys::U8string& result)
{
    str::utf32to8(s.c_str(), s.size(), result);
}
static inline str::U8string utf8_(uint32_t ch)
{
    const std::u32string s{static_cast<std::u32string::value_type>(ch)};
    str::U8string retval;
    utf32to8(s, retval);
    return retval;
};
static const std::map<std::u32string::value_type, sys::U8string> Windows1252_x80_x9F_to_u8string{
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
    , {0x91, utf8_(0x2018)  } // LEFT SINGLE QUOTATION MARK
    , {0x92, utf8_(0x2019)  } // RIGHT SINGLE QUOTATION MARK
    , {0x93, utf8_(0x201C)  } // LEFT DOUBLE QUOTATION MARK
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

static std::map<std::u32string::value_type, sys::U8string> Windows1252_to_u8string()
{
    auto retval = Windows1252_x80_x9F_to_u8string;

    // Add the ISO8859-1 values to the map too.  1) We're already looking
    // in the map anyway for Windows-1252 characters. 2) Need map
    // entires for conversion from UTF-8 to Windows-1252.
    for (uint32_t ch_ = 0xA0; ch_ <= 0xff; ch_++)
    {
        // ISO8859-1 can be converted to UTF-8 with bit-twiddling
        const auto ch = static_cast<uint8_t>(ch_);

        // https://stackoverflow.com/questions/4059775/convert-iso-8859-1-strings-to-utf-8-in-c-c
        // *out++=0xc2+(*in>0xbf), *out++=(*in++&0x3f)+0x80;
        sys::U8string s{cast(0xc2 + (ch > 0xbf)), cast((ch & 0x3f) + 0x80)};  // ISO8859-1
        retval[ch_] = std::move(s);    
    }

    return retval;
}

template<typename TChar>
static void fromWindows1252_(uint8_t ch, std::basic_string<TChar>& result)
{
    // ASCII is the same in UTF-8
    if (ch < 0x80)
    {
        using value_type = typename std::basic_string<TChar>::value_type;
        result += static_cast<value_type>(ch);  // ASCII
    }
    else
    {
        using const_pointer = typename std::basic_string<TChar>::const_pointer;

        static const auto map = Windows1252_to_u8string();
        const auto it = map.find(ch);
        if (it != map.end())
        {
            result += str::c_str<const_pointer>(it->second); // TODO: avoid extra copy
        }
        else
        {
            // If the input text contains a character that isn't defined in
            // Windows-1252; return a "replacement character."  Yes, this will
            // **corrupt** the input data as information is lost:
            // https://en.wikipedia.org/wiki/Specials_(Unicode_block)#Replacement_character
            static const sys::U8string replacement_character_ = utf8_(0xfffd);
            static const std::basic_string<TChar> replacement_character = str::c_str<const_pointer>(replacement_character_);
            result += replacement_character;
        }
    }
}
template<typename TChar>
void windows1252to8_(str::W1252string::const_pointer p, size_t sz, std::basic_string<TChar>& result)
{
    for (size_t i = 0; i < sz; i++)
    {
        fromWindows1252_(static_cast<uint8_t>(p[i]), result);
    }
}
void str::windows1252to8(W1252string::const_pointer p, size_t sz, sys::U8string& result)
{
    windows1252to8_(p, sz, result);
}

template<typename TKey, typename TValue>
std::map<TValue, TKey> kv_to_vk(const std::map<TKey, TValue>& kv)
{
    std::map<TValue, TKey> retval;
    for (const auto& p : kv)
    {
        retval[p.second] = p.first;
    }
    return retval;
}

static void get_next_utf8_byte(str::U8string::const_pointer p, size_t sz,
    size_t& i,  str::U8string& utf8)
{
    if (!(i + i < sz))
    {
        throw std::invalid_argument("No remaining bytes, invalid UTF-8 encoding.");
    }
    i++;  // move to next byte

    // Bytes 2, 3 and 4 are always >= 0x80 (10xxxxxx), see https://en.wikipedia.org/wiki/UTF-8
    const auto b = static_cast<uint8_t>(p[i]);
    if (b < static_cast<uint8_t>(0x80))  // 10xxxxxx
    {
        throw std::invalid_argument("Invalid next byte in UTF-8 encoding.");
    }
    utf8 += str::U8string{cast(b)};
}

template<typename TChar>
void toWindows1252_(str::U8string::const_pointer p, size_t sz, std::basic_string<TChar>& result)
{
    using value_type = typename std::basic_string<TChar>::value_type;
    for (size_t i = 0; i < sz; i++)
    {
        const auto b1 = static_cast<uint8_t>(p[i]);

        // ASCII is the same in UTF-8
        if (b1 < 0x80) // 0xxxxxxx
        {
            result += static_cast<value_type>(b1);  // ASCII
            continue;
        }

        auto utf8 = str::U8string{cast(b1)};

        get_next_utf8_byte(p, sz, i, utf8);
        if (b1 >= 0xE0)  // 1110xxxx
        {
            // should be a 3- or 4-byte sequence
            get_next_utf8_byte(p, sz, i, utf8);      
            if (b1 >= 0xF0)  // 1111xxx
            {
                // should be a 4-byte sequence
                get_next_utf8_byte(p, sz, i, utf8);     
            }
        }

        static const auto map = kv_to_vk(Windows1252_to_u8string());
        const auto it = map.find(utf8);
        if (it != map.end())
        {
            result += static_cast<value_type>(it->second);
        }
        else
        {
            assert("UTF-8 sequence can't be converted to Windows-1252." && 0);
            result += static_cast<TChar>(0x7F);  // <DEL>
        }
    }
}
// Exposing for unit-tests
static void toWindows1252(str::U8string::const_pointer p, size_t sz, str::W1252string& result)
{
    toWindows1252_(p, sz, result);
}
str::W1252string str::details::toWindows1252(const str::U8string& utf8)
{
    str::W1252string retval;
    ::toWindows1252(utf8.c_str(), utf8.size(), retval);
    return retval;
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

static void toString_(sys::U8string::const_pointer pUtf8, size_t sz, std::string& result)
{
    const auto pUtf8_ = str::cast<std::string::const_pointer>(pUtf8);

    auto platform = str::details::Platform;  // "conditional expression is constant"
    if (platform == str::details::PlatformType::Windows)
    {
        toWindows1252_(pUtf8, sz, result);
    }
    else if (platform == str::details::PlatformType::Linux)
    {
        result = pUtf8_;  // copy
    }
    else
    {
        throw std::logic_error("Unknown platform.");
    }
}
void str::details::toString(sys::U8string::const_pointer pUtf8, std::string& result)
{
    const auto pUtf8_ = str::cast<std::string::const_pointer>(pUtf8);
    const auto sz = strlen(pUtf8_);
    toString_(pUtf8, sz, result);
}
template <>
std::string str::toString(const str::U8string& utf8)
{
    std::string retval;
    toString_(utf8.c_str(), utf8.size(), retval);  // TODO: avoid call to strlen()
    return retval;
}

// Maybe someday "native" will be std::u8string on all platforms?
static std::string toNative_(const str::W1252string& w1252)
{
    auto platform = str::details::Platform;  // "conditional expression is constant"
    if (platform == str::details::PlatformType::Windows)
    {
        return str::c_str<std::string::const_pointer>(w1252);  // copy
    }
    if (platform == str::details::PlatformType::Linux)
    {
        std::string retval;
        windows1252to8_(w1252.c_str(), w1252.size(), retval);
        return retval;
    }

    throw std::logic_error("Unknown platform.");
}
void str::details::toNative(const str::W1252string& w1252, std::string& result)
{
    result = toNative_(w1252);
}

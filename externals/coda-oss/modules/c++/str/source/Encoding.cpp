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
inline coda_oss::u8string utf8_(std::u32string::value_type ch)
{
    return str::to_u8string(std::u32string{ch});
};
static const std::map<std::u32string::value_type, coda_oss::u8string> Windows1252_x80_x9F_to_u8string{
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

static std::map<std::u32string::value_type, coda_oss::u8string> Windows1252_to_u8string()
{
    auto retval = Windows1252_x80_x9F_to_u8string;

    // Add the ISO8859-1 values to the map too.  1) We're already looking
    // in the map anyway for Windows-1252 characters. 2) Need map
    // entires for conversion from UTF-8 to Windows-1252.
    for (std::u32string::value_type ch = 0xA0; ch <= 0xff; ch++)
    {
        // ISO8859-1 can be converted to UTF-8 with bit-twiddling
      
        // https://stackoverflow.com/questions/4059775/convert-iso-8859-1-strings-to-utf-8-in-c-c
        // *out++=0xc2+(*in>0xbf), *out++=(*in++&0x3f)+0x80;
        const auto b1 = 0xc2 + (ch > 0xbf);
        const auto b2 = (ch & 0x3f) + 0x80;
        coda_oss::u8string s {static_cast<coda_oss::u8string::value_type>(b1)};
        s += coda_oss::u8string {static_cast<coda_oss::u8string::value_type>(b2)};
        retval[ch] = std::move(s);    
    }

    return retval;
}

inline void append(std::string& result, const coda_oss::u8string& utf8)
{
    result += str::c_str<std::string::const_pointer>(utf8);
}
inline void append(coda_oss::u8string& result, const coda_oss::u8string& utf8)
{
    result += utf8;
}

template<typename TChar>
static void fromWindows1252_(str::W1252string::value_type ch, std::basic_string<TChar>& result)
{
    // ASCII is the same in UTF-8
    if (ch < static_cast<str::W1252string::value_type>(0x80))
    {
        using value_type = typename std::basic_string<TChar>::value_type;
        result += static_cast<value_type>(ch);  // ASCII
        return;
    }

    using const_pointer = typename std::basic_string<TChar>::const_pointer;
    static const auto map = Windows1252_to_u8string();
    const auto it = map.find(static_cast<std::u32string::value_type>(ch));
    if (it != map.end())
    {
        append(result, it->second);
        return;
    }

    // If the input text contains a character that isn't defined in
    // Windows-1252; return a "replacement character."  Yes, this will
    // **corrupt** the input data as information is lost:
    // https://en.wikipedia.org/wiki/Specials_(Unicode_block)#Replacement_character
    static const coda_oss::u8string replacement_character_ = utf8_(0xfffd);
    static const std::basic_string<TChar> replacement_character = str::c_str<const_pointer>(replacement_character_);
    result += replacement_character;
}
template<typename TChar>
std::basic_string<TChar>& windows1252to8(str::W1252string::const_pointer p, size_t sz, std::basic_string<TChar>& result)
{
    for (size_t i = 0; i < sz; i++)
    {
        fromWindows1252_(static_cast<str::W1252string::value_type>(p[i]), result);
    }
    return result;
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

static void get_next_utf8_byte(coda_oss::u8string::const_pointer p, size_t sz,
    size_t& i,  coda_oss::u8string& utf8)
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
    utf8 += coda_oss::u8string{static_cast<coda_oss::u8string::value_type>(b)};
}
template<typename TChar>
static void utf8to1252(coda_oss::u8string::const_pointer p, size_t sz, std::basic_string<TChar>& result)
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

        auto utf8 = coda_oss::u8string{static_cast<coda_oss::u8string::value_type>(b1)};

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

struct back_inserter final
{ 
    coda_oss::u8string* container = nullptr; // pointer instead of reference for copy
    explicit back_inserter(coda_oss::u8string& s) noexcept : container(&s) { }

    back_inserter& operator=(uint8_t v)
    {
        container->push_back(static_cast<coda_oss::u8string::value_type>(v));
        return *this;
    }
    back_inserter& operator*() noexcept { return *this; }
    back_inserter operator++(int) noexcept { return *this; }
};

coda_oss::u8string str::to_u8string(std::u16string::const_pointer p, size_t sz)
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
    coda_oss::u8string retval;
    utf8::utf16to8(p, p + sz, back_inserter(retval));
    return retval;

    /*
    std::vector<unsigned short> utf16line;
    auto begin = c_str<const uint8_t*>(result);
    utf8::utf8to16(begin, begin+result.size(), std::back_inserter(utf16line));
    */
}
std::string& str::details::to_u8string(std::u16string::const_pointer p, size_t sz, std::string& result)
{
    utf8::utf16to8(p, p + sz, std::back_inserter(result));
    return result;
}

coda_oss::u8string str::to_u8string(std::u32string::const_pointer p, size_t sz)
{
    coda_oss::u8string retval;
    utf8::utf32to8(p, p + sz, back_inserter(retval));
    return retval;
}
std::string& str::details::to_u8string(std::u32string::const_pointer p, size_t sz, std::string& result)
{
    utf8::utf32to8(p, p + sz, std::back_inserter(result));
    return result;
}


coda_oss::u8string str::to_u8string(W1252string::const_pointer p, size_t sz)
{
    coda_oss::u8string retval;
    windows1252to8(p, sz, retval);
    return retval;
}

std::string& str::details::to_u8string(std::string::const_pointer p, size_t sz, bool is_utf8 /* is 'p' UTF-8? */, std::string& result)
{
    // https://en.cppreference.com/w/cpp/language/operator_assignment#Builtin_direct_assignment
    // "...  and returns an lvalue identifying the left operand after modification."
    return is_utf8 ? (result = p) : windows1252to8(cast<W1252string::const_pointer>(p), sz, result);
}

coda_oss::u8string str::details::to_u8string(std::string::const_pointer p, size_t sz, bool is_utf8 /* is 'p' UTF-8? */)
{
    return is_utf8 ?
        cast<coda_oss::u8string::const_pointer>(p) :  // copy
        to_u8string(cast<W1252string::const_pointer>(p), sz);
}
coda_oss::u8string str::to_u8string(std::string::const_pointer p, size_t sz)
{
    auto platform = details::Platform;  // "conditional expression is constant"
    return details::to_u8string(p, sz, platform == details::PlatformType::Linux); // std::string is UTF-8 on Linux
}

str::W1252string str::details::to_w1252string(coda_oss::u8string::const_pointer p, size_t sz)
{
    str::W1252string retval;
    utf8to1252(p, sz, retval);
    return retval;
}

str::W1252string str::details::to_w1252string(std::string::const_pointer p, size_t sz, bool is_utf8 /* is 's' UTF-8? */)
{
    return is_utf8 ?
        to_w1252string(cast<coda_oss::u8string ::const_pointer>(p), sz) :
        cast<str::W1252string ::const_pointer>(p);  // copy
}
str::W1252string str::details::to_w1252string(std::string::const_pointer p, size_t sz)
{
    auto platform = details::Platform;  // "conditional expression is constant"
    return to_w1252string(p, sz, platform == details::PlatformType::Linux); // std::string is UTF-8 on Linux
}

std::string str::details::to_native(coda_oss::u8string::const_pointer p, size_t sz)
{
    auto platform = str::details::Platform;  // "conditional expression is constant"
    if (platform == str::details::PlatformType::Windows)
    {
        std::string retval;
        utf8to1252(p, sz, retval);
        return retval;
    }
    if (platform == str::details::PlatformType::Linux)
    {
        return cast<std::string::const_pointer>(p);  // copy
    }
    throw std::logic_error("Unknown platform.");
}

std::string str::details::to_native(W1252string::const_pointer p, size_t sz)
{
    auto platform = details::Platform;  // "conditional expression is constant"
    if (platform == details::PlatformType::Windows)
    {    
        return cast<std::string::const_pointer>(p);  // copy
    }
    if (platform == details::PlatformType::Linux)
    {
        std::string retval;
        windows1252to8(p, sz, retval);
        return retval;
    }
    throw std::logic_error("Unknown platform.");
}

coda_oss::u8string str::fromWindows1252(std::string::const_pointer p, size_t sz)
{
    return to_u8string(cast<str::W1252string::const_pointer>(p), sz);
}

coda_oss::u8string str::fromUtf8(std::string::const_pointer p, size_t sz)
{
    return to_u8string(cast<coda_oss::u8string::const_pointer>(p), sz);
}

template <>
std::string str::toString(const coda_oss::u8string& utf8)
{
    return str::details::to_native(utf8);
}
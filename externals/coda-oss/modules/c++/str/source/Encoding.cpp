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
#include "str/Encoding.h"

#include <assert.h>
#include <string.h>
#if _WIN32
#include <comdef.h>  // _bstr_t
#endif

#include <map>
#include <locale>
#include <stdexcept>
#include <vector>
#include <iterator>

#include "gsl/gsl.h"
#include "config/compiler_extensions.h"

#include "str/Manip.h"
#include "str/Convert.h"
CODA_OSS_disable_warning_push
#if _MSC_VER
#pragma warning(disable: 26818) // Switch statement does not cover all cases. Consider adding a '...' label (es.79).
#else
CODA_OSS_disable_warning(-Wshadow)
#endif
#include "str/utf8.h"
CODA_OSS_disable_warning_pop

//// "sys" depends on "str" so can't use sys::PlatformType
//enum class PlatformType
//{
//    Windows,
//    Linux,
//    // MacOS
//};
//#if defined(_WIN32)
//static constexpr auto Platform = PlatformType::Windows;
//#elif defined(_POSIX_C_SOURCE)
//static constexpr auto Platform = PlatformType::Linux;
//#else
//#error "Unknown platform"
//#endif

// Need to look up characters from \x80 (EURO SIGN) to \x9F (LATIN CAPITAL LETTER Y WITH DIAERESIS)
// in a map: http://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WINDOWS/CP1252.TXT
inline coda_oss::u8string utf8_(uint32_t i)
{
    const auto ch = gsl::narrow<std::u32string::value_type>(i);
    return str::to_u8string(std::u32string{ch});
}

static const auto& Windows1252_x80_x9F_to_u8string()
{
    static const std::map<uint32_t, coda_oss::u8string> retval {
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
    return retval;
}

static auto Windows1252_to_u8string()
{
    auto retval = Windows1252_x80_x9F_to_u8string();

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
    result += str::c_str<std::string>(utf8);
}
inline void append(coda_oss::u8string& result, const coda_oss::u8string& utf8)
{
    result += utf8;
}
template<typename CharT>
inline void append(std::basic_string<CharT>& result, const coda_oss::u8string& utf8)
{
    auto p = str::c_str<std::string>(utf8);
    utf8::utf8to16(p, p + utf8.size(), std::back_inserter(result));
}
inline void append(std::u32string& result, const coda_oss::u8string& utf8)
{
    auto p = str::c_str<std::string>(utf8);
    utf8::utf8to32(p, p + utf8.size(), std::back_inserter(result));
}

template<typename TChar>
static void fromWindows1252_(str::W1252string::value_type ch, std::basic_string<TChar>& result, bool strict=false)
{
    // ASCII is the same in UTF-8
    if (ch < static_cast<str::W1252string::value_type>(0x80))
    {
        using value_type = typename std::basic_string<TChar>::value_type;
        result += static_cast<value_type>(ch);  // ASCII
        return;
    }

    static const auto map = Windows1252_to_u8string();
    const auto ch32 = static_cast<std::u32string::value_type>(ch);
    const auto it = map.find(ch32);
    if (it != map.end())
    {
        append(result, it->second);
        return;
    }

    switch (static_cast<uint8_t>(ch))
    {
    case 0x81:
    case 0x8d:
    case 0x8f:
    case 0x90:
    case 0x9d:
    {
        if (strict)
        {
            // If the input text contains a character that isn't defined in Windows-1252; return a
            // "replacement character."  Yes, this will  **corrupt** the input data as information is lost:
            // https://en.wikipedia.org/wiki/Specials_(Unicode_block)#Replacement_character
            static const coda_oss::u8string replacement_character = utf8_(0xfffd);
            append(result, replacement_character);
        }
        else
        {
            // _bstr_t just preserves these values, do the same
            append(result, utf8_(ch32));
        }
        break;
    }
    default:
        throw std::invalid_argument("Invalid Windows-1252 character.");
    }
}
template<typename TChar>
void w1252_to_string(str::W1252string::const_pointer p, size_t sz, std::basic_string<TChar>& result)
{
    for (size_t i = 0; i < sz; i++)
    {
        fromWindows1252_(p[i], result);
    }
}
template<typename TReturn>
inline TReturn to_Tstring(str::W1252string::const_pointer p, size_t sz)
{
    TReturn retval;
    w1252_to_string(p, sz, retval);
    return retval;
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
    if (!(i + 1 < sz))
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
static void utf8to1252(coda_oss::u8string::const_pointer p, size_t sz, std::basic_string<TChar>& result, bool strict=false)
{
    using value_type = TChar;
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
        else if (strict)
        {
            throw std::invalid_argument("UTF-8 sequence can't be converted to Windows-1252.");
            //assert("UTF-8 sequence can't be converted to Windows-1252." && 0);
            //result += static_cast<TChar>(0x7F);  // <DEL>
        }
        else
        {
            // _bstr_t preserves these values
            if (utf8.length() == 2)
            {
                result += static_cast<value_type>(utf8[1]);
            }
            else
            {
                assert("UTF-8 sequence can't be converted to Windows-1252." && 0);
                result += static_cast<value_type>(0x7F);  // <DEL>
            }
        }
    }
}

static auto u16_to_Windows1252()
{
    // Find the corresponding UTF-16 value for every Windows-1252 input;
    // obviously, most UTF-16 values can't be converted.  Skip the first half
    // as they're the same for ASCII.
    std::map<std::u16string::value_type, str::W1252string::value_type> retval;
    for (uint16_t i = 0x0080; i <= 0x00ff; i++)  // **not** `uint8_t` to avoid wrap-around
    {
        const auto ch = static_cast<str::W1252string::value_type>(i);
        const auto u16 = str::to_u16string(&ch, 1);
        assert(u16.length() == 1);
        retval[u16[0]] = ch;
    }
    return retval;
}
static inline void utf16to1252(std::u16string::const_pointer p, size_t sz, std::string& result, bool strict=false)
{
    using value_type = std::string::value_type;

    static const auto map = u16_to_Windows1252();
    for (size_t i = 0; i < sz; i++)
    {
        const auto ch = p[i];

        if (ch < 0x0080) // ASCII
        {
            result += gsl::narrow<value_type>(ch);  
            continue;
        }

        const auto it = map.find(ch);
        if (it != map.end())
        {
            result += static_cast<value_type>(it->second);
        }
        else if (strict)
        {
            throw std::invalid_argument("UTF-16 sequence can't be converted to Windows-1252.");
        }
        else
        {
            assert("UTF-16 sequence can't be converted to Windows-1252." && 0);
            result += static_cast<value_type>(0x7F);  // <DEL>
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

template <typename TBasicStringT, typename CharT>
inline auto to_uXXstring(const std::basic_string<CharT>& s)
{
    const auto p = str::c_str<TBasicStringT>(s);
    #ifdef _WIN32
    return str::to_u16string(p, s.length()); // std::wstring is UTF-16 on Windows
    #else
    return str::to_u32string(p, s.length()); // assume std::wstring is UTF-32 everywhere except Windows
    #endif
}
template<typename CharT>
static std::wstring toWString_(const std::basic_string<CharT>& s, bool is_utf8)
{
   
    const auto result = is_utf8 ? to_uXXstring<coda_oss::u8string>(s)
                                : to_uXXstring<str::W1252string>(s);
    return str::c_str<std::wstring>(result);  // copy
}

/***********************************************************************************/

std::string str::toString(const coda_oss::u8string& s)
{
    #if _WIN32
    std::string retval;
    utf8to1252(s.c_str(), s.length(), retval);
    return retval;
    #else
    return str::c_str<std::string>(s); // cast & copy
    #endif
}

std::string str::toString(const str::W1252string& s)
{
    #if _WIN32
    return str::c_str<std::string>(s); // cast & copy
    #else
    return to_Tstring<std::string>(s.c_str(), s.length());
    #endif
}

std::string str::toString(const std::wstring& s)
{
    const auto sz = s.length();

    std::string retval;
    #if _WIN32
    const auto p = c_str<std::u16string>(s); // std::wstring is UTF-16 on Windows
    utf16to1252(p, sz, retval); // UTF16 -> Windows-1252 on Windows.
    #else
    const auto p = c_str<std::u32string>(s); // assume std::wstring is UTF-32 on any non-Windows platform
    utf8::utf32to8(p, p + sz, std::back_inserter(retval)); // UTF32 -> UTF-8 everywhere else.
    #endif   
    return retval;
}

std::wstring str::toWString(const std::string& s)
{
     #if _WIN32
    return toWString_(s, false /*is_utf8*/); // Input is Windows-1252 on Windows
    #else
    return toWString_(s, true /*is_utf8*/);  // Input is UTF-8 everywhere except Windows
    #endif
}
std::wstring str::toWString(const coda_oss::u8string& s)
{
    return toWString_(s, true /*is_utf8*/);
}
std::wstring str::toWString(const str::W1252string& s)
{
    return toWString_(s, false /*is_utf8*/);
}

coda_oss::u8string str::u8FromString(const std::string& s)
{
    #if _WIN32
    const auto p = c_str<str::W1252string>(s); // std::string is Windows-1252 on Windows
    #else
    const auto p = c_str<coda_oss::u8string>(s); // assume std::string is UTF-8 on any non-Windows platform
    #endif
    return str::to_u8string(p, s.length());
}

coda_oss::u8string str::u8FromWString(const std::wstring& s)
{
    #if _WIN32
    const auto p = c_str<std::u16string>(s); // std::wstring is UTF-16 on Windows
    #else
    const auto p = c_str<std::u32string>(s); // assume std::wstring is UTF-32 on any non-Windows platform
    #endif   
    return str::to_u8string(p, s.length());
}

/***********************************************************************************/

std::u16string str::to_u16string(str::W1252string::const_pointer p, size_t sz)
{
    auto retval = to_Tstring<std::u16string>(p, sz);
    #if defined(_WIN32) && (!defined(_NDEBUG) || defined(DEBUG))
    const _bstr_t bstr(std::string(str::cast<const char*>(p), sz).c_str()); // no _bstr_t ctor taking sz
    const std::wstring wstr(static_cast<const wchar_t*>(bstr));
    assert(retval == str::cast<std::u16string::const_pointer>(wstr.c_str()));
    #endif
    return retval;
}
std::u32string str::to_u32string(str::W1252string::const_pointer p, size_t sz)
{
    return to_Tstring<std::u32string>(p, sz);
}

str::W1252string str::to_w1252string(coda_oss::u8string::const_pointer p, size_t sz)
{
    str::W1252string retval;
    utf8to1252(p, sz, retval);
    return retval;
}

coda_oss::u8string str::to_u8string(std::u16string::const_pointer p, size_t sz)
{
    coda_oss::u8string retval;
    utf8::utf16to8(p, p + sz, back_inserter(retval));
    return retval;
}

std::u16string str::to_u16string(coda_oss::u8string::const_pointer p_, size_t sz)
{
    auto p = str::cast<std::string::const_pointer>(p_);
    std::u16string retval;
    utf8::utf8to16(p, p + sz, std::back_inserter(retval));
    return retval;
}

std::u32string str::to_u32string(coda_oss::u8string::const_pointer p_, size_t sz)
{
    auto p = str::cast<std::string::const_pointer>(p_);
    std::u32string retval;
    utf8::utf8to32(p, p + sz, std::back_inserter(retval));
    return retval;
}

coda_oss::u8string str::to_u8string(std::u32string::const_pointer p, size_t sz)
{
    coda_oss::u8string retval;
    utf8::utf32to8(p, p + sz, back_inserter(retval));
    return retval;
}

coda_oss::u8string str::to_u8string(W1252string::const_pointer p, size_t sz)
{
    return to_Tstring<coda_oss::u8string>(p, sz);
}

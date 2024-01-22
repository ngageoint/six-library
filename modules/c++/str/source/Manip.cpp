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
#include "str/Manip.h"

#include <limits.h>
#include <stdio.h>
#include <wctype.h>
#include <assert.h>

#include <iostream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <cctype>
#include <array>

#include "gsl/gsl.h"

#include "str/Convert.h"
#include "str/Encoding.h"

namespace
{
inline char transformCheck(int c, int (*transform)(int))
{
    // Ensure the character can be represented
    // as an unsigned char or is 'EOF', as the
    // behavior for all other characters is undefined
    if ((c >= 0 && c <= UCHAR_MAX) || c == EOF)
    {
        return static_cast<char>(transform(c));
    }
    else
    {
        // Invalid char for transform: no-op
        return static_cast<char>(c);
    }
}

inline char tolowerCheck(char c)
{
    return transformCheck(c, tolower);
}

inline char toupperCheck(char c)
{
    return transformCheck(c, toupper);
}
}


namespace str
{

// TODO: https://stackoverflow.com/questions/31959532/best-way-to-remove-white-spaces-from-stdstring
template<typename TChar>
inline void trim_(std::basic_string<TChar> & s)
{
    size_t i;
    for (i = 0; i < s.length(); i++)
    {
        if (!iswspace(static_cast<wint_t>(s[i])))
            break;
    }
    s.erase(0, i);

    for (i = s.length() - 1; static_cast<int>(i) >= 0; i--)
    {
        if (!iswspace(static_cast<wint_t>(s[i])))
            break;

    }
    if (i + 1 < s.length())
        s.erase(i + 1);
}
void trim(std::string& s)
{
    trim_(s);
}
std::string trim(const std::string& str)
{
    auto retval = str;
    trim(retval);
    return retval;
}
void trim(coda_oss::u8string& s)
{
    trim_(s);
}
coda_oss::u8string trim(const coda_oss::u8string& str)
{
    auto retval = str;
    trim(retval);
    return retval;
}

bool ends_with(const std::string& s, const std::string& match) noexcept
{
    const size_t mLen = match.length();
    const size_t sLen = s.length();
    for (size_t i = 0; i < sLen && i < mLen; ++i)
        if (!(s[sLen - i - 1] == match[mLen - i - 1]))
            return false;
    return sLen >= mLen;
}
bool endsWith(const std::string& s, const std::string& match)
{
    return ends_with(s, match);
}

bool starts_with(const std::string& s, const std::string& match) noexcept
{
    const size_t mLen = match.length();
    const size_t sLen = s.length();
    for (size_t i = 0; i < sLen && i < mLen; ++i)
        if (!(s[i] == match[i]))
            return false;
    return sLen >= mLen;
}
bool startsWith(const std::string& s, const std::string& match)
{
    return starts_with(s, match);
}

size_t replace(std::string& str,
        const std::string& search,
        const std::string& replace,
        size_t start)
{
    size_t index = str.find(search, start);

    if (index != std::string::npos)
    {
        // ASAN error: str.replace(index, search.length(), replace);
        str = str.substr(0, index) + replace + str.substr(index + search.length());
        start = index;
    }
    else
    {
        start = str.length();
    }

    return start;        
}

void replaceAll(std::string& string,
                const std::string& search,
                const std::string& replace)
{
    size_t start = 0;
    while (start < string.length())
    {
        start = str::replace(string, search, replace, start);
        // skip ahead --
        // avoids inifinite loop if replace contains search 
        start += replace.length();                             
    }
}

bool contains(const std::string& str, const std::string& match)
{
    return str.find(match) != std::string::npos;
}

static inline bool isTest(const std::string& s, int (*is)(int))
{
    for (const auto& ch : s)
    {
        if (!is(ch))
            return false;
    }
    return !s.empty();
}

bool isAlpha(const std::string& s)
{
    return isTest(s, isalpha);
}

template<typename Pred>
static inline bool isTest(const std::string& s, int (*is1)(int), Pred is2)
{
    for (const auto& ch : s)
    {
        if (!is1(ch) && !is2(ch))
            return false;
    }
    return !s.empty();
}

bool isAlphaSpace(const std::string& s)
{
    return isTest(s, isalpha, isspace);
}

bool isNumeric(const std::string& s)
{
    return isTest(s, isdigit);
}

bool isNumericSpace(const std::string& s)
{
    return isTest(s, isdigit, isspace);
}

bool isWhitespace(const std::string& s)
{
    for (const auto& ch : s)
    {
        if (!isspace(ch))
            return false;
    }
    return true;
}

bool isAlphanumeric(const std::string& s)
{
    return isTest(s, isalpha, isdigit);
}

bool isAsciiPrintable(const std::string& s)
{
    for (const auto& c : s)
    {
        if (c < 32 || c > 126)
            return false;
    }
    return true;
}

bool containsOnly(const std::string& s, const std::string& validChars)
{
    typedef std::string::const_iterator StringIter;
    std::vector<bool> chars(255, false);
    for (StringIter it = validChars.begin(); it != validChars.end(); ++it)
    {
        const auto i = gsl::narrow<ptrdiff_t>(*it);
        chars[i] = true;
    }
    for (StringIter it = s.begin(); it != s.end(); ++it)
    {
        const auto i = gsl::narrow<ptrdiff_t>(*it);
        if (!chars[i])
            return false;
    }
    return true;
}

std::vector<std::string> split(const std::string& s,
        const std::string& splitter, size_t maxSplit)
{
    std::vector < std::string > vec;
    const auto str_l = s.length();
    const auto split_l = splitter.length();
    size_t pos = 0;
    while (pos < str_l && maxSplit != 1)
    {
        auto nextPos = s.find(splitter, pos);
        if (nextPos == std::string::npos)
            nextPos = str_l;
        if (nextPos != pos)
            vec.push_back(s.substr(pos, nextPos - pos));
        pos = nextPos + split_l;
        if (maxSplit != std::string::npos && vec.size() >= maxSplit - 1)
            break;
    }

    if (pos < str_l)
        vec.push_back(s.substr(pos));

    return vec;
}

// Calling ::toupper() can be slow as the CRT might check for locales.
// Since we only have 256 values, a lookup table is very fast and doesn't
// use much memory.
static const auto& make_lookup(std::array<uint8_t, UINT8_MAX + 1>& result,
                               char (*to)(char))
{
    // For each of 256 values, record the corresponding tolower/toupper value;
    // this makes converting very fast as no checking or arithmetic must be done.
    for (size_t i = 0; i <= 0xff; i++)
    {
        const auto ch = to(static_cast<char>(i));
        result[i] = static_cast<uint8_t>(ch);
    }
    return result;
}

template<typename TChar>
static void do_lookup(std::basic_string<TChar>& s, const std::array<uint8_t, UINT8_MAX + 1>& lookup)
{
    for (auto& ch : s)
    {
        const auto i = static_cast<uint8_t>(ch);
        ch = static_cast<TChar>(lookup[i]);
    }
}

void ascii_upper(std::string& s)
{
    static std::array<uint8_t, UINT8_MAX + 1> lookup_;
    static const auto& lookup = make_lookup(lookup_, toupperCheck);
    do_lookup(s, lookup);
}

void ascii_lower(std::string& s)
{
    static std::array<uint8_t, UINT8_MAX + 1> lookup_;
    static const auto& lookup = make_lookup(lookup_, tolowerCheck);
    do_lookup(s, lookup);
}

inline char to_w1252_upper_(char ch)
{
    if ((ch >= 'a') && (ch <= 'z'))
    {
        return ch ^ 0x20;  // ('a' - 'A');
    }

    // See chart at: https://en.wikipedia.org/wiki/Windows-1252
    constexpr uint8_t s_with_caron = 0x9a /* š */;
    constexpr uint8_t oe = 0x9c /* œ */;
    constexpr uint8_t z_with_caron = 0x9e /* ž */;
    constexpr uint8_t a_with_grave = 0xe0 /* à */;
    constexpr uint8_t o_with_diaeresis = 0xf6 /* ö */;
    constexpr uint8_t o_with_slash = 0xf8 /* ø */;
    constexpr uint8_t small_thorn = 0xfe /* þ */;
    constexpr uint8_t y_with_diaeresis = 0xff /* ÿ */;

    const auto u8 = static_cast<uint8_t>(ch);
    if ((u8 == s_with_caron) || (u8 == oe) || (u8 == z_with_caron))
    {
        return ch ^ 0x10;    
    }
    if ((u8 >= a_with_grave) && (u8 <= o_with_diaeresis))
    {
        return ch ^ 0x20;
    }
    if ((u8 >= o_with_slash) && (u8 <= small_thorn))
    {
        return ch ^ 0x20;
    }
    if (u8 == y_with_diaeresis)
    {
        constexpr uint8_t Y_with_diaeresis = 0x9f /* Ÿ */;
        return Y_with_diaeresis;
    }

    return ch;
}
str::Windows1252_T to_w1252_upper(str::Windows1252_T ch)
{
    const auto retval = to_w1252_upper_(static_cast<char>(ch));
    return static_cast<str::Windows1252_T>(retval);
}

inline char to_w1252_lower_(char ch)
{
    if ((ch >= 'A') && (ch <= 'Z'))
    {
        return ch | 0x20;
    }

    constexpr uint8_t S_with_caron = 0x8a /* Š */;
    constexpr uint8_t OE = 0x8c /*Œ */;
    constexpr uint8_t Z_with_caron = 0x8e /* Ž */;
    constexpr uint8_t Y_with_diaeresis = 0x9f /* Ÿ */;
    constexpr uint8_t A_with_grave = 0xc0 /* À */;
    constexpr uint8_t O_with_diaeresis = 0xd6 /* Ö */;
    constexpr uint8_t O_with_slash = 0xd8 /* Ø */;
    constexpr uint8_t capital_thorn = 0xde /* Þ */;

    const auto u8 = static_cast<uint8_t>(ch);
    if ((u8 == S_with_caron) || (u8 == OE) || (u8 == Z_with_caron))
    {
        return ch | 0x10;
    }
    if (u8 == Y_with_diaeresis)
    {
        constexpr uint8_t y_with_diaeresis = 0xff /* ÿ */;
        return y_with_diaeresis;
    }
    if ((u8 >= A_with_grave) && (u8 <= O_with_diaeresis))
    {
        return ch | 0x20;
    }
    if ((u8 >= O_with_slash) && (u8 <= capital_thorn))
    {
        return ch | 0x20;
    }
    return ch;
}
str::Windows1252_T to_w1252_lower(str::Windows1252_T ch)
{
    const auto retval = to_w1252_lower_(static_cast<char>(ch));
    return static_cast<str::Windows1252_T>(retval);
}

void escapeForXML(std::string& str)
{
    // & needs to be first or else it'll mess up the other characters that we replace
    replaceAll(str, "&", "&amp;");
    replaceAll(str, "<", "&lt;");
    replaceAll(str, ">", "&gt;");
    replaceAll(str, "\"", "&quot;");
    replaceAll(str, "'", "&apos;");
    replaceAll(str, "\n", "&#10;");
    replaceAll(str, "\r", "&#13;");
}

// https://en.cppreference.com/w/cpp/string/char_traits
class ci_char_traits final : public std::char_traits<char>
{
    // Use our own routine rather than strcasecmp() so that the same
    // toupperCheck() is used as when calling upper().
    static auto to_upper(char ch) noexcept
    {
        return toupperCheck(ch);
    }

    static int compare(const char* s1, const char* s2, std::size_t n) noexcept
    {
        while (n-- != 0)
        {
            if (to_upper(*s1) < to_upper(*s2))
                return -1;
            if (to_upper(*s1) > to_upper(*s2))
                return 1;
            ++s1;
            ++s2;
        }
        return 0;
    }

    public:
    static int compare(const std::string& s1, const std::string& s2) noexcept
    {
        if (s1.length() < s2.length())
        {
            return -1;
        }
        if (s1.length() > s2.length())
        {
            return 1;
        }
        assert(s1.length() == s2.length());
        return compare(s1.c_str(), s2.c_str(), s1.length());
    }
};

bool eq(const std::string& lhs, const std::string& rhs) noexcept
{
    return ci_char_traits::compare(lhs, rhs) == 0;
}
bool ne(const std::string& lhs, const std::string& rhs) noexcept
{
    return ci_char_traits::compare(lhs, rhs) != 0;
}

}

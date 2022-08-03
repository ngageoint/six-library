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

#ifndef __STR_MANIP_H__
#define __STR_MANIP_H__

#include <ctype.h>
#include <wchar.h>

#include <string>
#include <vector>

#include "config/compiler_extensions.h"
#include "config/Exports.h"
#include "coda_oss/CPlusPlus.h"
#include "coda_oss/string.h"
#include "str/Convert.h"

namespace str
{

CODA_OSS_disable_warning_push
#if _MSC_VER
#pragma warning(disable: 26460) //The reference argument 's' for function 'str::data<char>' can be marked as const (con.3).
#endif
 // non-const overload for .data() in C++17
template<typename CharT>
inline CharT* data(std::basic_string<CharT>& s) noexcept
{
    #if CODA_OSS_cpp17
    return s.data();
    #else
    CODA_OSS_disable_warning_push
    #if _MSC_VER
    #pragma warning(disable : 26492)  // Don't use const_cast to cast away const or volatile (type.3).
    #endif  
    return const_cast <typename std::basic_string<CharT>::pointer>(s.data());
    CODA_OSS_disable_warning_pop
    #endif // CODA_OSS_cpp17
}
CODA_OSS_disable_warning_pop
template <typename CharT>
inline const CharT* data(const std::basic_string<CharT>& s) noexcept // to make generic programming easier
{
    return s.data();
}

/**
 *  Trim the white space off the back and front of a string
 *  @param  s  String to trim
 */
CODA_OSS_API void trim(std::string& s);
CODA_OSS_API std::string trim(const std::string& s);
CODA_OSS_API void trim(coda_oss::u8string& s);
CODA_OSS_API coda_oss::u8string trim(const coda_oss::u8string& s);

/**
 *  Checks the end of s with match
 *  @param  s  String to check
 *  @param  match  String to compare with
 *  @return true if it matches, otherwise false
 */
CODA_OSS_API bool endsWith(const std::string& s, const std::string& match);
CODA_OSS_API bool ends_with(const std::string& s, const std::string&) noexcept;

/**
 *  Checks the start of s with match
 *  @param  s  String to check
 *  @param  s  String to compare with
 *  @return true if it matches, otherwise false
 */
CODA_OSS_API bool startsWith(const std::string& s, const std::string& match);
CODA_OSS_API bool starts_with(const std::string& s, const std::string&) noexcept;


/**
 *  finds the first instance of "search" and
 *  substitutes it for "replace", it then returns
 *  the start of search, so you can loop and replace
 *  all instances of "search". 
 *  @param  str     String to check
 *  @param  search  String to search for
 *  @param  replace String to replace with
 *  @param  start   starting position to start search
 *  @return position of first find, str.length() if not found
 */
CODA_OSS_API size_t replace(std::string& str, 
               const std::string& search,
               const std::string& replace,
               size_t start = 0);

/**
 *  finds all instances of "search" and
 *  substitutes them for "replace"
 *  @param  string  String to check
 *  @param  search  String to search for
 *  @param  replace String to replace with
 */
CODA_OSS_API void replaceAll(std::string& string, 
                const std::string& search,
                const std::string& replace);

/**
 * Returns true if the string contains the match
 */
CODA_OSS_API bool contains(const std::string& str, const std::string& match);


/**
 * Returns true if the string contains only letters.
 */
CODA_OSS_API bool isAlpha(const std::string& s);

/**
 * Returns true if the string contains only letters and spaces.
 */
CODA_OSS_API bool isAlphaSpace(const std::string& s);

/**
 * Returns true if the string contains only digits. This does not include
 * decimal points.
 */
CODA_OSS_API bool isNumeric(const std::string& s);

/**
 * Returns true if the string contains only digits and spaces.
 */
CODA_OSS_API bool isNumericSpace(const std::string& s);

/**
 * Returns true if the string contains only whitespace characters (or empty).
 */
CODA_OSS_API bool isWhitespace(const std::string& s);

/**
 * Returns true if the string contains only letters and digits.
 */
CODA_OSS_API bool isAlphanumeric(const std::string& s);

/**
 * Returns true if the string contains only ASCII printable characters.
 */
CODA_OSS_API bool isAsciiPrintable(const std::string& s);

/**
 * Returns true if the string contains only the given allowed characters.
 */
CODA_OSS_API bool containsOnly(const std::string& s, const std::string& validChars);

/**
 *  Splits a string based on a splitter string. Similar to tokenization, except
 *  the splitter string can be of any length.
 *  @param  s         String to check
 *  @param  splitter  String to split upon
 *  @return vector of strings
 */
CODA_OSS_API std::vector<std::string> split(const std::string& s,
                               const std::string& splitter = " ",
                               size_t maxSplit = std::string::npos);

//! Uses std::transform to convert all chars to lower case
//! Uses std::transform to convert all chars to upper case
CODA_OSS_API void lower(std::string& s);
CODA_OSS_API void upper(std::string& s);

/*!
 * Replaces any characters that are invalid in XML (&, <, >, ', ") with their
 * escaped counterparts
 */
CODA_OSS_API void escapeForXML(std::string& str);

template<typename T>
inline std::string join(const std::vector<T>& toks, const std::string& with)
{
    if (toks.empty())
        return "";

    const auto len = static_cast<int>(toks.size());
    std::ostringstream oss;
    int i = 0;
    for (; i < len - 1; i++)
    {
        oss << str::toString<T>(toks[i]) << with;
    }
    oss << str::toString(toks[i]);
    return oss.str();
}


}

#endif

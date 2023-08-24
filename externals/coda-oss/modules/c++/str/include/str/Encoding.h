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

#pragma once
#ifndef CODA_OSS_str_Encoding_h_INCLUDED_
#define CODA_OSS_str_Encoding_h_INCLUDED_

#include <string.h>
#include <wchar.h>
#include <stdint.h>

#include <memory>
#include <string>
#include <type_traits>

#include "coda_oss/string.h"
#include "gsl/gsl.h"
#include "config/Exports.h"
#include "str/W1252string.h"

namespace str
{
template <typename TReturn, typename TChar>
inline TReturn cast(const TChar* s)
{
    // This is OK as UTF-8 can be stored in std::string
    // Note that casting between the string types will CRASH on some
    // implementations. NO: reinterpret_cast<const std::string&>(value)
    const void* const pStr = s;
    auto const retval = static_cast<TReturn>(pStr);
    static_assert(sizeof(*retval) == sizeof(*s), "sizeof(*TReturn) != sizeof(*TChar)"); 
    return retval;
}
template <typename TBasicStringT, typename TChar>
inline typename TBasicStringT::const_pointer c_str(const std::basic_string<TChar>& s)
{
    using return_t = typename TBasicStringT::const_pointer;
    return cast<return_t>(s.c_str());
}

// When the encoding is important, we want to "traffic" in coda_oss::u8string (UTF-8), not
// str::W1252string (Windows-1252) or std::string (unknown).  Make it easy to get those from other encodings.
CODA_OSS_API coda_oss::u8string to_u8string(str::W1252string::const_pointer, size_t);
CODA_OSS_API coda_oss::u8string to_u8string(std::u16string::const_pointer, size_t);
CODA_OSS_API coda_oss::u8string to_u8string(std::u32string::const_pointer, size_t);
inline coda_oss::u8string to_u8string(coda_oss::u8string::const_pointer p, size_t sz)
{
    return coda_oss::u8string(p, sz);
}

// UTF-16 is the default on Windows.
CODA_OSS_API std::u16string to_u16string(coda_oss::u8string::const_pointer, size_t);
CODA_OSS_API std::u16string to_u16string(str::W1252string::const_pointer, size_t);

// UTF-32 is convenient because each code-point is a single 32-bit integer.
// It's typically std::wstring::value_type on Linux, but NOT Windows.
CODA_OSS_API std::u32string to_u32string(coda_oss::u8string::const_pointer, size_t);
CODA_OSS_API std::u32string to_u32string(str::W1252string::const_pointer, size_t);

// Windows-1252 (almost the same as ISO8859-1) is the default single-byte encoding on Windows.
CODA_OSS_API str::W1252string to_w1252string(coda_oss::u8string::const_pointer p, size_t sz);
inline auto to_w1252string(const coda_oss::u8string& s)
{
    return to_w1252string(s.c_str(), s.length());
}

CODA_OSS_API coda_oss::u8string to_u8string(std::string::const_pointer, size_t); // platform determines Windows-1252 or UTF-8 input
CODA_OSS_API coda_oss::u8string to_u8string(std::wstring::const_pointer, size_t); // platform determines UTF16 or UTF-32 input

template<typename CharT>
inline auto to_u8string(const std::basic_string<CharT>& s)
{
    return to_u8string(s.c_str(), s.length());
}
template <typename CharT>
inline auto to_u16string(const std::basic_string<CharT>& s)
{
    return to_u16string(s.c_str(), s.length());
}
template <typename CharT>
inline auto to_u32string(const std::basic_string<CharT>& s)
{
    return to_u32string(s.c_str(), s.length());
}

template <typename CharT>
inline coda_oss::u8string to_u8string(const CharT* p)
{
    return to_u8string(std::basic_string<CharT>(p));
}

}

#endif // CODA_OSS_str_Encoding_h_INCLUDED_

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

#ifndef CODA_OSS_str_Encoding_h_INCLUDED_
#define CODA_OSS_str_Encoding_h_INCLUDED_
#pragma once

#include <string.h>
#include <wchar.h>
#include <stdint.h>

#include <memory>
#include <string>
#include <type_traits>

#include "coda_oss/string.h"
#include "gsl/gsl.h"

// This can be useful for code that will compile on all platforms, but needs
// different platform-specific behavior.  This avoids the use of more #ifdefs
// (no preprocessor) and also squelches compiler-warnings about unused local
// functions.
namespace str { namespace details  // YOU should be using sys::PlatformType
{
enum class PlatformType
{
    Windows,
    Linux,
    // MacOS
};

#if _WIN32
constexpr auto Platform = PlatformType::Windows;
#else
constexpr auto Platform = PlatformType::Linux;
#endif
} }


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
template <typename TReturn, typename TChar>
inline TReturn c_str(const std::basic_string<TChar>& s)
{
    return cast<TReturn>(s.c_str());
}

// This is to make it difficult to get encodings mixed up; it's here (in a .h
// file) as we want to unit-test it. Windows1252_T for Windows-1252 characters
enum class Windows1252_T : unsigned char { };  // https://en.cppreference.com/w/cpp/language/types
using W1252string = std::basic_string<Windows1252_T>;  // https://en.cppreference.com/w/cpp/string

coda_oss::u8string fromWindows1252(std::string::const_pointer, size_t); // std::string is Windows-1252 **ON ALL PLATFORMS**
inline coda_oss::u8string fromWindows1252(std::string::const_pointer s)
{
    return fromWindows1252(s, gsl::narrow<size_t>(strlen(s)));
}
coda_oss::u8string fromUtf8(std::string::const_pointer, size_t); // std::string is UTF-8 **ON ALL PLATFORMS**
inline coda_oss::u8string fromUtf8(std::string::const_pointer s)
{
    return fromUtf8(s, gsl::narrow<size_t>(strlen(s)));
}

//////////////////////////////////////////////////////////////////////////////////////////

// We'll get strange errors, possibibly at link-time, if wchar_t is not a wchar_t type.
// MSVC has an option to control this: https://docs.microsoft.com/en-us/cpp/build/reference/zc-wchar-t-wchar-t-is-native-type
// https://en.cppreference.com/w/cpp/language/types
// "It has the same size, signedness, and alignment as one of the integer types, but is a distinct type."
static_assert(!std::is_same<wchar_t, uint16_t>::value, "wchar_t should not be the same as uint16_t");
static_assert(!std::is_same<wchar_t, int16_t>::value, "wchar_t should not be the same as int16_t");
static_assert(!std::is_same<wchar_t, uint32_t>::value, "wchar_t should not be the same as uint32_t");
static_assert(!std::is_same<wchar_t, int32_t>::value, "wchar_t should not be the same as int32_t");

// When the encoding is important, we want to "traffic" in coda_oss::u8string (UTF-8), not
// str::W1252string (Windows-1252) or std::string (unknown).  Make it easy to get those from other encodings.
coda_oss::u8string to_u8string(std::string::const_pointer, size_t);  // std::string is Windows-1252 or UTF-8  depending on platform
coda_oss::u8string to_u8string(str::W1252string::const_pointer, size_t);
inline coda_oss::u8string to_u8string(coda_oss::u8string::const_pointer s, size_t sz)
{
    return coda_oss::u8string(s, sz);
}
coda_oss::u8string to_u8string(std::wstring::const_pointer, size_t);  // std::wstring is UTF-16 or UTF-32  depending on platform

// UTF-16 is typically uses on Windows (where it is std::wstring::value_type);
// Linux preferred UTF-32.
coda_oss::u8string to_u8string(std::u16string::const_pointer, size_t);
std::u16string to_u16string(coda_oss::u8string::const_pointer, size_t);

// UTF-32 is convenient because each code-point is a single 32-bit integer.
// It's typically std::wstring::value_type on Linux, but NOT Windows.
coda_oss::u8string to_u8string(std::u32string::const_pointer, size_t);
std::u32string to_u32string(coda_oss::u8string::const_pointer, size_t);

template <typename TChar>
inline coda_oss::u8string to_u8string(const std::basic_string<TChar>& s)
{
    return to_u8string(s.c_str(), s.size());
}
template <typename TChar>
inline std::u16string to_u16string(const std::basic_string<TChar>& s)
{
    return to_u16string(s.c_str(), s.size());
}
template <typename TChar>
inline std::u32string to_u32string(const std::basic_string<TChar>& s)
{
    return to_u32string(s.c_str(), s.size());
}

namespace details // YOU should use EncodedStringView
{
coda_oss::u8string to_u8string(std::string::const_pointer, size_t, bool is_utf8 /* is 's' UTF-8? */);
std::u16string to_u16string(std::string::const_pointer, size_t, bool is_utf8 /* is 's' UTF-8? */);
std::u32string to_u32string(std::string::const_pointer, size_t, bool is_utf8 /* is 's' UTF-8? */);
std::wstring to_wstring(std::string::const_pointer, size_t, bool is_utf8 /* is 's' UTF-8? */);

std::string& to_u8string(std::string::const_pointer, size_t, bool is_utf8 /* is 's' UTF-8? */, std::string&); // encoding is lost
std::string& to_u8string(std::u16string::const_pointer, size_t, std::string&); // encoding is lost
std::string& to_u8string(std::u32string::const_pointer, size_t, std::string&); // encoding is lost

str::W1252string to_w1252string(std::string::const_pointer, size_t); // std::string is Windows-1252 or UTF-8  depending on platform
str::W1252string to_w1252string(std::string::const_pointer, size_t, bool is_utf8 /* is 's' UTF-8? */);
str::W1252string to_w1252string(coda_oss::u8string::const_pointer, size_t);
inline str::W1252string to_w1252string(str::W1252string::const_pointer s, size_t sz)
{
    return str::W1252string(s, sz);
}

std::string to_native(coda_oss::u8string::const_pointer, size_t); // std::string is Windows-1252 or UTF-8  depending on platform
std::string to_native(str::W1252string::const_pointer s, size_t sz); // std::string is Windows-1252 or UTF-8  depending on platform
inline std::string to_native(std::string::const_pointer s, size_t sz, bool is_utf8 /* is 's' UTF-8? */) // std::string is Windows-1252 or UTF-8  depending on platform
{
    return is_utf8 ? to_native(cast<coda_oss::u8string::const_pointer>(s), sz)
                   : to_native(cast<str::W1252string::const_pointer>(s), sz);
}
inline std::string to_native(std::string::const_pointer s, size_t sz)
{
    return std::string(s, sz);
}
template <typename TChar>
inline std::string to_native(const std::basic_string<TChar>& s)
{
    return to_native(s.c_str(), s.size());
}
}
}

#endif // CODA_OSS_str_Encoding_h_INCLUDED_

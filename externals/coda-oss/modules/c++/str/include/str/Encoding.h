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

#ifndef CODA_OSS_Encoding_h_INCLUDED_
#define CODA_OSS_Encoding_h_INCLUDED_
#pragma once

#include <wchar.h>

#include <memory>
#include <string>

// This is a fairly low-level file, so don't #include a lot of our other files
#include "str/String_.h"


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
void windows1252to8(W1252string::const_pointer, size_t, sys::U8string&); // c.f. utf16to8

// assume std::string is Windows-1252 **ON ALL PLATFORMS**
sys::U8string fromWindows1252(std::string::const_pointer, size_t);
inline sys::U8string fromWindows1252(const std::string& s)
{
    return fromWindows1252(s.c_str(), s.size());
}

// assume std::string is UTF-8 **ON ALL PLATFORMS**
sys::U8string fromUtf8(std::string::const_pointer, size_t);
inline sys::U8string fromUtf8(const std::string& s)
{
    return fromUtf8(s.c_str(), s.size());
}

//////////////////////////////////////////////////////////////////////////////////////////
// These use utf8:: routines; see utf8.h
void utf16to8(std::u16string::const_pointer, size_t, sys::U8string&);
inline void utf16to8(const std::u16string& s, sys::U8string& result)
{
    utf16to8(s.c_str(), s.size(), result);
}

void utf32to8(std::u32string::const_pointer, size_t, sys::U8string&);
inline void utf32to8(const std::u32string& s, sys::U8string& result)
{
    utf32to8(s.c_str(), s.size(), result);
}

inline void strto8(std::u16string::const_pointer p, size_t sz, sys::U8string& result)
{
    utf16to8(p, sz, result);
}
inline void strto8(const std::u16string& s, sys::U8string& result)
{
    utf16to8(s, result);
}
inline void strto8(std::u32string::const_pointer p, size_t sz, sys::U8string& result)
{
    utf32to8(p, sz, result);
}
inline void strto8(const std::u32string& s, sys::U8string& result)
{
    utf32to8(s, result);
}

/*
Using the utf8:: library for these causes compiler-errors on some platforms.
We don't really need them anyway: Linux has good support for UTF-8 and
our existing code-bases don't use UTF-16 on Windows; so UTF-8 to 
Windows-1252 will be good enough.

void utf8to16(sys::U8string::const_pointer, size_t, std::u16string&);
inline void utf8to16(const sys::U8string& s, std::u16string& result)
{
    utf8to16(s.c_str(), s.size(), result);
}

void utf8to32(sys::U8string::const_pointer, size_t, std::u32string&);
inline void utf8to32(const sys::U8string& s, std::u32string& result)
{
    utf8to32(s.c_str(), s.size(), result);
}
*/

//////////////////////////////////////////////////////////////////////////////////////////

// When the encoding is important, we want to "traffic" in sys::U8string (UTF-8), not
// str::W1252string (Windows-1252) or std::string (unknown).  Make it easy to get those from other encodings.
sys::U8string to_u8string(std::string::const_pointer, size_t);  // assume Windows-1252 or UTF-8  based on platform
sys::U8string to_u8string(str::W1252string::const_pointer, size_t);
inline sys::U8string to_u8string(sys::U8string::const_pointer s, size_t sz)
{
    return sys::U8string(s, sz);
}
template <typename TChar>
inline sys::U8string to_u8string(const std::basic_string<TChar>& s)
{
    return to_u8string(s.c_str(), s.size());
}

}

#endif // CODA_OSS_Encoding_h_INCLUDED_

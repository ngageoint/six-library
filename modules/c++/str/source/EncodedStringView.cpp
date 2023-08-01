/* =========================================================================
 * This file is part of str-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2022, Maxar Technologies, Inc.
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

#include "str/EncodedStringView.h"

#include <assert.h>
#include <string.h>

#include <stdexcept>
#include "coda_oss/span.h"

#include "str/Convert.h"
#include "str/Encoding.h"
#include "str/EncodedString.h"

enum class PlatformType
{
    Windows,
    Linux,
    // MacOS
};

#ifdef _WIN32
static auto Platform = PlatformType::Windows;
#else
static auto Platform = PlatformType::Linux;
#endif

inline std::u16string to_u16string(std::string::const_pointer s, size_t sz, bool is_utf8 /* is 's' UTF-8? */)
{
    if (is_utf8)
    {
        return str::to_u16string(str::cast<coda_oss::u8string::const_pointer>(s), sz);
    }
    return str::to_u16string(str::cast<str::W1252string::const_pointer>(s), sz);
}

static std::string to_native(coda_oss::u8string::const_pointer p, size_t sz)
{
    if (Platform == PlatformType::Windows)
    {
        std::string retval;
        str::details::utf8to1252(p, sz, retval);
        return retval;
    }
    if (Platform == PlatformType::Linux)
    {
        return std::string(str::cast<std::string::const_pointer>(p), sz);
    }
    throw std::logic_error("Unknown platform.");
}

static std::string to_native(str::W1252string::const_pointer p, size_t sz)
{
    if (Platform == PlatformType::Windows)
    {    
        return std::string(str::cast<std::string::const_pointer>(p), sz);
    }
    if (Platform == PlatformType::Linux)
    {
        std::string retval;
        str::details::w1252to8(p, sz, retval);
        return retval;
    }
    throw std::logic_error("Unknown platform.");
}

template <typename Char8T>
inline auto make_span(const Char8T* s, size_t c)
{
    static_assert(sizeof(Char8T) == sizeof(char), "sizeof(Char8T) != sizeof(char)"); 
    return coda_oss::span<const Char8T>(s, c);
}
template <typename Char8T>
inline auto make_span(const Char8T* s)
{
    auto const s_ = str::cast<const char*>(s);
    return make_span(s, strlen(s_));
}
template <typename Char8T>
inline auto make_span(const std::basic_string<Char8T>& s)
{
    assert(strlen(str::c_str<std::string>(s)) == s.length());
    return make_span(s.c_str(), s.length());
}
template <typename Char8T>
inline auto make_span(coda_oss::span<const Char8T> s)
{
    auto const s_ = str::cast<const char*>(s.data());
    assert(strlen(s_) == s.size());
    return coda_oss::span<const char>(s_, s.size());
}

str::EncodedStringView::EncodedStringView(std::string::const_pointer p, std::string::size_type c) : mString(make_span(p, c)) { }
str::EncodedStringView::EncodedStringView(std::string::const_pointer p) : mString(make_span(p)) { }
str::EncodedStringView::EncodedStringView(const std::string& s) : mString(make_span(s)){ }

str::EncodedStringView::EncodedStringView(coda_oss::span<const coda_oss::u8string::value_type> s) : mString(make_span(s)), mIsUtf8(true) {}
str::EncodedStringView::EncodedStringView(coda_oss::u8string::const_pointer p, coda_oss::u8string::size_type c) : EncodedStringView(make_span(p, c)) { }
str::EncodedStringView::EncodedStringView(coda_oss::u8string::const_pointer p) : EncodedStringView(make_span(p)) { }
str::EncodedStringView::EncodedStringView(const coda_oss::u8string& s) : EncodedStringView(make_span(s)) { }

str::EncodedStringView::EncodedStringView(coda_oss::span<const str::W1252string::value_type> s) : mString(make_span(s)), mIsUtf8(false) {}
str::EncodedStringView::EncodedStringView(str::W1252string::const_pointer p, str::W1252string::size_type c) : EncodedStringView(make_span(p, c)) { }
str::EncodedStringView::EncodedStringView(str::W1252string::const_pointer p) :  EncodedStringView(make_span(p)) { }
str::EncodedStringView::EncodedStringView(const str::W1252string& s) : EncodedStringView(make_span(s)) { }

std::string str::EncodedStringView::native() const
{
    const auto s = mString.data();
    const auto sz = mString.size();
    return mIsUtf8 ? to_native(str::cast<coda_oss::u8string::const_pointer>(s), sz)
                   : to_native(str::cast<str::W1252string::const_pointer>(s), sz);
}

coda_oss::u8string str::EncodedStringView::u8string() const
{
    return mIsUtf8 ?
        str::cast<coda_oss::u8string::const_pointer>(mString.data()) :  // copy
        str::to_u8string(str::cast<str::W1252string::const_pointer>(mString.data()), mString.size());
}
std::string str::EncodedStringView::asUtf8() const
{
    const auto result = u8string();
    return str::c_str<std::string>(result);  // cast & copy
}

std::u16string str::EncodedStringView::u16string() const
{
    return ::to_u16string(mString.data(), mString.size(), mIsUtf8);
}

inline std::u32string to_u32string(std::string::const_pointer s, size_t sz, bool is_utf8 /* is 's' UTF-8? */)
{
    if (is_utf8)
    {
        return str::to_u32string(str::cast<coda_oss::u8string::const_pointer>(s), sz);
    }
    return str::to_u32string(str::cast<str::W1252string::const_pointer>(s), sz);
}
std::u32string str::EncodedStringView::u32string() const
{
    return ::to_u32string(mString.data(), mString.size(), mIsUtf8);
}

std::wstring str::EncodedStringView::wstring() const  // UTF-16 on Windows, UTF-32 on Linux
{
    const auto p = mString.data();
    const auto sz = mString.size();
    const auto s =
    // Need to use #ifdef's because str::cast() checks to be sure the sizes are correct.
    #ifdef _WIN32
    ::to_u16string(p, sz, mIsUtf8);  // std::wstring is UTF-16 on Windows
    #else
    ::to_u32string(p, sz, mIsUtf8);  // std::wstring is UTF-32 on Linux
    #endif    
    return str::c_str<std::wstring>(s); // copy
}

str::W1252string str::EncodedStringView::w1252string() const
{
    return mIsUtf8 ?
        str::to_w1252string(str::cast<coda_oss::u8string ::const_pointer>(mString.data()), mString.size()) :
        str::cast<str::W1252string ::const_pointer>(mString.data());  // copy
}
std::string str::EncodedStringView::asWindows1252() const
{
    const auto result = w1252string();
    return str::c_str<std::string>(result); // cast & copy
}

bool str::EncodedStringView::operator_eq(const EncodedStringView& rhs) const
{
    auto& lhs = *this;
   
    // if encoding is the same, strcmp() will work
    if (lhs.mIsUtf8 == rhs.mIsUtf8) // both are UTF-8 or both are Windows-1252
    {
        // But we can avoid that call if the pointers are the same
        const auto pLhs = lhs.mString.data();
        const auto pRhs = rhs.mString.data();
        if ((pLhs == pRhs) && (rhs.mString.size() == rhs.mString.size()))
        {
            return true;
        }
        return strcmp(pLhs, pRhs) == 0;
    }

    // LHS and RHS have different encodings, but one must be UTF-8
    assert((lhs.mIsUtf8 && !rhs.mIsUtf8) || (!lhs.mIsUtf8 && rhs.mIsUtf8)); // should have used strcmp(), above
    auto& utf8 = lhs.mIsUtf8 ? lhs : rhs;
    auto& w1252 = !lhs.mIsUtf8 ? lhs : rhs;

    // If UTF-8 is native on this platform, convert to UTF-8; otherwise do a native comparision
    return mNativeIsUtf8 ? utf8.c_u8str() == w1252.u8string() : utf8.native() == w1252.mString.data();
}



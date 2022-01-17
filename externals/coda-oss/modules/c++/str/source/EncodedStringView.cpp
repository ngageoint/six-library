/* =========================================================================
 * This file is part of str-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2022, Maxar Technologies, Inc.
 *
 * xml.lite-c++ is free software; you can redistribute it and/or modify
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
#include "coda_oss/memory.h"
#include "coda_oss/span.h"

#include "str/Convert.h"
#include "str/Encoding.h"

template <typename CharT>
inline coda_oss::span<const char> make_span(const CharT* s)
{
    // Be sure we can cast between the different types
    static_assert(sizeof(*s) == sizeof(std::string::value_type), "wrong size for CharT");
    static_assert(sizeof(*s) == sizeof(sys::U8string::value_type), "wrong size for CharT");
    static_assert(sizeof(*s) == sizeof(str::W1252string::value_type), "wrong size for CharT");
    auto s_ = str::cast<const char*>(s);
    return coda_oss::span<const char>(s_, strlen(s_));
}

template<typename CharT>
inline coda_oss::span<const char> make_span(const std::basic_string<CharT>& s)
{
    // Be sure we can cast between the different types
    static_assert(sizeof(s[0]) == sizeof(std::string::value_type), "wrong size for CharT");
    static_assert(sizeof(s[0]) == sizeof(sys::U8string::value_type), "wrong size for CharT");
    static_assert(sizeof(s[0]) == sizeof(str::W1252string::value_type), "wrong size for CharT");
    return coda_oss::span<const char>(str::c_str<const char*>(s), s.size());
}

str::EncodedStringView::EncodedStringView(std::string::const_pointer p) : mString(make_span(p)) { }
str::EncodedStringView::EncodedStringView(sys::U8string::const_pointer p) : mString(make_span(p)), mIsUtf8(true) { }
str::EncodedStringView::EncodedStringView(str::W1252string::const_pointer p) :  mString(make_span(p)), mIsUtf8(false) { }
str::EncodedStringView::EncodedStringView(const std::string& s) : mString(make_span(s)){ }
str::EncodedStringView::EncodedStringView(const sys::U8string& s) : mString(make_span(s)), mIsUtf8(true) { }
str::EncodedStringView::EncodedStringView(const str::W1252string& s) : mString(make_span(s)), mIsUtf8(false) { }

static std::string utf8_to_native(coda_oss::span<const char> s)
{
    std::string retval;
    str::details::toString(str::cast<sys::U8string::const_pointer>(s.data()), retval);
    return retval;
}
static std::string w1252_to_native(coda_oss::span<const char> s)
{
    // This internal helper routine will convert from Windows-1252 to UTF-8 on Linux
    std::string retval;
    str::details::toNative(str::cast<str::W1252string::const_pointer>(s.data()), retval);
    return retval;
}
std::string str::EncodedStringView::native() const
{
    return mIsUtf8 ? utf8_to_native(mString) : w1252_to_native(mString);
}

inline sys::U8string utf8_to_u8string(coda_oss::span<const char> s)
{
    return str::cast<sys::U8string::const_pointer>(s.data());
}
inline sys::U8string w1252_to_u8string(coda_oss::span<const char> s)
{
    return str::to_u8string(str::cast<str::W1252string::const_pointer>(s.data()), s.size());
}
sys::U8string str::EncodedStringView::to_u8string() const
{
    return mIsUtf8 ? utf8_to_u8string(mString) : w1252_to_u8string(mString);
}

bool str::EncodedStringView::operator_eq(const EncodedStringView& rhs) const
{
    auto& lhs = *this;
   
    // if encoding is the same, strcmp() will work
    if (lhs.mIsUtf8 == rhs.mIsUtf8) // both are UTF-8 or both are Windows-1252
    {
        // But we can avoid that call if the pointers are the same
        if ((lhs.mString.data() == rhs.mString.data()) && (rhs.mString.size() == rhs.mString.size()))
        {
            return true;
        }
        return strcmp(lhs.mString.data(), rhs.mString.data()) == 0;
    }

    // LHS and RHS have different encodings, but one must be UTF-8
    assert((lhs.mIsUtf8 && !rhs.mIsUtf8) || (!lhs.mIsUtf8 && rhs.mIsUtf8)); // should have used strcmp(), above
    auto& utf8 = lhs.mIsUtf8 ? lhs : rhs;
    auto& w1252 = !lhs.mIsUtf8 ? lhs : rhs;

    // If UTF-8 is native on this platform, convert to UTF-8; otherwise do a native comparision
    return mNativeIsUtf8 ? str::cast<str::U8string::const_pointer>(utf8.mString.data()) == w1252.to_u8string() : 
        utf8.native() == w1252.mString.data();
}

std::string& str::EncodedStringView::toUtf8(std::string& result) const
{
    // This is easy, but creates "unneeded" sys::U8string; it would be
    // better to put the result directly into std::string
    const auto utf8 = to_u8string(); // TODO: avoid this copy
    result = str::c_str<std::string::const_pointer>(utf8);
    return result;
}

namespace str
{
// GCC wants specializations outside of the class.  We need these here (now)
// anyway for access to pImpl.
template <>
std::string::const_pointer EncodedStringView::cast() const
{
    return mString.data();
}
template <>
sys::U8string::const_pointer EncodedStringView::cast() const
{
    return mIsUtf8 ? str::cast<sys::U8string::const_pointer>(mString.data()) : nullptr;
}
template <>
str::W1252string::const_pointer EncodedStringView::cast() const
{
    return mIsUtf8 ? nullptr : str::cast<str::W1252string::const_pointer>(mString.data());
}
} // namespace str


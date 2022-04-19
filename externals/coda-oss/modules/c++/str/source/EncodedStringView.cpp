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
#include "coda_oss/memory.h"
#include "coda_oss/span.h"

#include "str/Convert.h"
#include "str/Encoding.h"
#include "str/EncodedString.h"

template <typename CharT>
inline coda_oss::span<const char> make_span(const CharT* s)
{
    auto s_ = str::cast<const char*>(s);
    return coda_oss::span<const char>(s_, strlen(s_));
}

template<typename CharT>
inline coda_oss::span<const char> make_span(const std::basic_string<CharT>& s)
{
    return coda_oss::span<const char>(str::c_str<const char*>(s), s.size());
}

str::EncodedStringView::EncodedStringView(std::string::const_pointer p) : mString(make_span(p)) { }
str::EncodedStringView::EncodedStringView(coda_oss::u8string::const_pointer p) : mString(make_span(p)), mIsUtf8(true) { }
str::EncodedStringView::EncodedStringView(str::W1252string::const_pointer p) :  mString(make_span(p)), mIsUtf8(false) { }
str::EncodedStringView::EncodedStringView(const std::string& s) : mString(make_span(s)){ }
str::EncodedStringView::EncodedStringView(const coda_oss::u8string& s) : mString(make_span(s)), mIsUtf8(true) { }
str::EncodedStringView::EncodedStringView(const str::W1252string& s) : mString(make_span(s)), mIsUtf8(false) { }

std::string str::EncodedStringView::native() const
{
    return str::details::to_native(mString.data(), mString.size(), mIsUtf8);
}

coda_oss::u8string str::EncodedStringView::u8string() const
{
    return str::details::to_u8string(mString.data(), mString.size(), mIsUtf8);
}
std::string& str::EncodedStringView::toUtf8(std::string& result) const
{
    return str::details::to_u8string(mString.data(), mString.size(), mIsUtf8, result);
}

std::u16string str::EncodedStringView::u16string() const
{
    return str::details::to_u16string(mString.data(), mString.size(), mIsUtf8);
}
str::ui16string str::EncodedStringView::ui16string_() const
{
    return str::details::to_ui16string(mString.data(), mString.size(), mIsUtf8);
}

std::u32string str::EncodedStringView::u32string() const
{
    return str::details::to_u32string(mString.data(), mString.size(), mIsUtf8);
}
std::wstring str::EncodedStringView::wstring() const  // UTF-16 on Windows, UTF-32 on Linux
{
    return str::details::to_wstring(mString.data(), mString.size(), mIsUtf8);
}


str::W1252string str::EncodedStringView::w1252string() const
{
    return str::details::to_w1252string(mString.data(), mString.size(), mIsUtf8);
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
    return mNativeIsUtf8 ? utf8.c_str() == w1252.u8string() : utf8.native() == w1252.mString.data();
}



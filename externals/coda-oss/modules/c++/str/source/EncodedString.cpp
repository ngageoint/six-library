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
#include "str/EncodedString.h"

#include <type_traits>

void str::EncodedString::assign(coda_oss::u8string::const_pointer s)
{
    using char_t = std::remove_pointer<decltype(s)>::type; // avoid copy-paste error
    using string_t = std::basic_string<std::remove_const<char_t>::type>;
    s_ = cast<std::string::const_pointer>(s);  // copy
    v_ = EncodedStringView(str::c_str<string_t>(s_));
}

void str::EncodedString::assign(str::W1252string::const_pointer s)
{
    using char_t = std::remove_pointer<decltype(s)>::type; // avoid copy-paste error
    using string_t = std::basic_string<std::remove_const<char_t>::type>;
    s_ = cast<std::string::const_pointer>(s);  // copy
    v_ = EncodedStringView(str::c_str<string_t>(s_));  // avoid copy-paste error
}

static str::EncodedStringView make_EncodedStringView(const std::string& s, bool isUtf8)
{
    if (isUtf8)
    {
        return str::EncodedStringView(str::c_str<coda_oss::u8string>(s));
    }

    // not UTF-8, assume Windows-1252
    return str::EncodedStringView(str::c_str<str::W1252string>(s));
}

str::EncodedString::EncodedString(std::string::const_pointer s) :  s_(s) /*copy*/, v_(s_)  { }
str::EncodedString::EncodedString(const std::string& s) :  s_(s) /*copy*/, v_(s_) { }
str::EncodedString::EncodedString() : EncodedString(""){ }

str::EncodedString::EncodedString(coda_oss::u8string::const_pointer s)
{
    assign(s);
}
str::EncodedString::EncodedString(const coda_oss::u8string& s) : EncodedString(s.c_str()) { }

str::EncodedString::EncodedString(str::W1252string::const_pointer s)
{
    assign(s);
}
str::EncodedString::EncodedString(const str::W1252string& s) : EncodedString(s.c_str()) { }

str::EncodedString::EncodedString(const std::u16string& s) : EncodedString(to_u8string(s)) { }
str::EncodedString::EncodedString(const std::u32string& s) : EncodedString(to_u8string(s)) { }

static inline coda_oss::u8string to_u8string_(std::wstring::const_pointer p_, size_t sz)  // std::wstring is UTF-16 or UTF-32  depending on platform
{
    const auto p =
    // Need to use #ifdef's because str::cast() checks to be sure the sizes are correct.
    #if _WIN32
    str::cast<std::u16string::const_pointer>(p_); // std::wstring is UTF-16 on Windows
    #endif
    #if !_WIN32
    str::cast<std::u32string::const_pointer>(p_); // std::wstring is UTF-32 on Linux
    #endif    
    return str::to_u8string(p, sz);
}

str::EncodedString::EncodedString(std::wstring::const_pointer s)  : EncodedString(to_u8string_(s, wcslen(s))) { }
str::EncodedString::EncodedString(const std::wstring& s) : EncodedString(to_u8string_(s.c_str(), s.size())) { }

// create from a view
str::EncodedString& str::EncodedString::operator=(const EncodedStringView& v)
{
    if (v.mIsUtf8)
    {
        assign(v.c_u8str());
    }
    else
    {
        // not UTF-8, assume Windows-1252
        auto p = cast<W1252string::const_pointer>(v.mString.data());
        assign(p);
    }

    return *this;
}
str::EncodedString::EncodedString(const EncodedStringView& v)
{
    *this = v;
}

str::EncodedString& str::EncodedString::operator=(const EncodedString& es)
{
    if (this != &es)
    {
        this->s_ = es.s_;  // copy
        this->v_ = make_EncodedStringView(s_, es.view().mIsUtf8);
    }
    return *this;
}
str::EncodedString::EncodedString(const EncodedString& es) 
{
    *this = es;
}

str::EncodedString& str::EncodedString::operator=(EncodedString&& es)
{
    if (this != &es)
    {
        this->s_ = std::move(es.s_);
        this->v_ = make_EncodedStringView(s_, es.view().mIsUtf8);
    }
    return *this;
}
str::EncodedString::EncodedString(EncodedString&& es)
{
    *this = std::move(es);
}

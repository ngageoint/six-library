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

void str::EncodedString::assign(std::string::const_pointer s)
{
    s_ = s; // copy
    v_ = EncodedStringView(s_);
}

void str::EncodedString::assign(coda_oss::u8string::const_pointer s)
{
    s_ = cast<std::string::const_pointer>(s);  // copy
    v_ = EncodedStringView(c_str<decltype(s)>(s_)); // avoid copy-paste error
}

void str::EncodedString::assign(str::W1252string::const_pointer s)
{
    s_ = cast<std::string::const_pointer>(s);  // copy
    v_ = EncodedStringView(c_str<decltype(s)>(s_)); // avoid copy-paste error
}

str::EncodedString::EncodedString(std::string::const_pointer s)
{
    assign(s);
}
str::EncodedString::EncodedString(const std::string& s) : EncodedString(s.c_str()) { }

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

str::EncodedString::EncodedString(std::wstring::const_pointer s)  : EncodedString(to_u8string(s, wcslen(s))) { }
str::EncodedString::EncodedString(const std::wstring& s) : EncodedString(to_u8string(s)) { }

// create from a view
str::EncodedString& str::EncodedString::operator=(const EncodedStringView& v)
{
    if (v.mIsUtf8)
    {
        assign(v.c_str());
    }
    else
    {
        auto p = cast<W1252string::const_pointer>(v.mString.data());
        assign(p);
    }
    return *this;
}
str::EncodedString::EncodedString(const EncodedStringView& v)
{
    *this = v;
}

str::EncodedString str::EncodedString::fromUtf8(const std::string& s)
{
    return str::EncodedStringView::fromUtf8(s);
}
str::EncodedString str::EncodedString::fromWindows1252(const std::string& s)
{
    return str::EncodedStringView::fromWindows1252(s);
}

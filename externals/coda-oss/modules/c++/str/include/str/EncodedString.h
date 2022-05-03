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

#ifndef CODA_OSS_str_EncodedString_h_INCLLUDED_
#define CODA_OSS_str_EncodedString_h_INCLLUDED_
#pragma once

#include <string>
#include <ostream>

 #include "str/EncodedStringView.h"

/*!
 * \file EncodedString.h
 * \brief A String that can be either UTF-8 or "native" (Windows-1252).
 * Unlike EncodedStringView, this hangs onto the underlying string.
 *
 * On Linux, there is good support for UTF-8, so a std::string encoded
 * as UTF-8 will display the "foreign" characters properly.  On Windows,
 * the preferred way to do that is by using UTF-16 (WCHAR, std::wstring),
 * but little (none?) of our existing code bases do that.  (On Linux, std::wstring
 * is typically UTF-32.)
 *
 */

namespace str
{
class EncodedString final
{
    std::string s_;
    // We can do most everything through the view, so keep one around.
    EncodedStringView v_;

    const std::string& string() const
    {
        return s_;
    }

    // No "public" operator=() for these; this class is mostly for storage and/or conversion,
    // not extensive manipulation.  Create a new instance and assign/move that.
    void assign(coda_oss::u8string::const_pointer);
    void assign(str::W1252string::const_pointer);
    
public:
    EncodedString() = default;
    ~EncodedString() = default;
    EncodedString(const EncodedString&);
    EncodedString& operator=(const EncodedString&);
    EncodedString(EncodedString&&) noexcept;
    EncodedString& operator=(EncodedString&&) noexcept;

    explicit EncodedString(const coda_oss::u8string& s);
    explicit EncodedString(coda_oss::u8string::const_pointer);
    explicit EncodedString(const str::W1252string&);
    explicit EncodedString(str::W1252string::const_pointer);
    explicit EncodedString(const std::string&);  // Assume platform native encoding: UTF-8 on Linux, Windows-1252 on Windows
    explicit EncodedString(std::string::const_pointer);  // Assume platform native encoding: UTF-8 on Linux, Windows-1252 on Windows
    explicit EncodedString(const std::u16string&); // converted to UTF-8 for storage
    //explicit EncodedString(std::u16string::const_pointer); // no wcslen() for std::u16string::value_type
    explicit EncodedString(const std::u32string&); // converted to UTF-8 for storage
    //explicit EncodedString(std::u32string::const_pointer); // no wcslen() for std::u32string::value_type
    explicit EncodedString(const std::wstring&);  // Assume platform native encoding: UTF-32 on Linux, UTF-16 on Windows
    explicit EncodedString(std::wstring::const_pointer);  // can call wcslen()

    // create from a view
    EncodedString(const EncodedStringView&);
    EncodedString& operator=(const EncodedStringView&);
    
    // Input is encoded as specified on all platforms.
    static EncodedString fromUtf8(const std::string&);
    static EncodedString fromWindows1252(const std::string&);
    static EncodedString fromUtf16(const std::wstring&); // not currently implemetned, no need
    static EncodedString fromUtf32(const std::wstring&); // not currently implemetned, no need
    
    // For "complex" operatations, use the view.
    const EncodedStringView& view() const
    {
        return v_;
    }

    // Regardless of what string we're looking at, return a string in platform
    // native encoding: UTF-8 on Linux, Windows-1252 on Windows; this
    // might result in string conversion.
    std::string native() const // c.f. std::filesystem::path::native()
    {
        return view().native();
    }

    // Convert (perhaps) whatever we're looking at to UTF-8
    coda_oss::u8string u8string() const  // c.f. std::filesystem::path::u8string()
    {
        return view().u8string();
    }
    //std::string& toUtf8(std::string&) const; // std::string is encoded as UTF-8, always.
    //str::W1252string w1252string() const;  // c.f. std::filesystem::path::u8string()

    // Convert whatever we're looking at to UTF-16 or UTF-32
    std::u16string u16string() const  // c.f. std::filesystem::path::u8string()
    {
        return view().u16string();
    }
    std::u32string u32string() const  // c.f. std::filesystem::path::u8string()
    {
        return view().u32string();
    }
    std::wstring wstring() const // UTF-16 on Windows, UTF-32 on Linux
    {
        return view().wstring();
    }

    struct details final
    {
        static const std::string& string(const EncodedString& es) // for unit-testing
        {
            return es.string();
        }
    };
};

inline bool operator==(const EncodedString& lhs, const EncodedStringView& rhs)
{
    return lhs.view() == rhs;
}
inline bool operator!=(const EncodedString& lhs, const EncodedStringView& rhs)
{
    return !(lhs == rhs);
}

inline bool operator==(const EncodedString& lhs, const EncodedString& rhs)
{
    return lhs == rhs.view();
}
inline bool operator!=(const EncodedString& lhs, const EncodedString& rhs)
{
    return !(lhs == rhs);
}

inline std::ostream& operator<<(std::ostream& os, const EncodedString& es)
{
    os << es.native();
    return os;
}

}
#endif  // CODA_OSS_str_EncodedString_h_INCLLUDED_

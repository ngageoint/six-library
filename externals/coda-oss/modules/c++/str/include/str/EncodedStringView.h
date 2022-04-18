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

#ifndef CODA_OSS_str_EncodedStringView_h_INCLLUDED_
#define CODA_OSS_str_EncodedStringView_h_INCLLUDED_
#pragma once

#include <string>
#include <ostream>

#include "coda_oss/span.h"
 #include "str/Encoding.h"

/*!
 * \file EncodedStringView.h
 * \brief A read-only "view" onto a string.
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
class EncodedString; // forward
class EncodedStringView final
{
    // Since we only support two encodings--UTF-8 (native on Linux) and Windows-1252
    // (native on Windows)--both of which are 8-bits, a simple "bool" flag will do.
    coda_oss::span<const char> mString;
    static constexpr bool mNativeIsUtf8 = details::Platform == details::PlatformType::Linux ? true : false;
    bool mIsUtf8 = mNativeIsUtf8;
    
    // Want to create an EncodedString from EncodedStringView.  The public interface
    // doesn't expose "mIsUtf8" so there's (intentinally) no way for clients to know the encoding.
    friend EncodedString;

    coda_oss::u8string::const_pointer c_str() const
    {
        return cast<coda_oss::u8string::const_pointer>(mString.data());
    }

    str::W1252string w1252string() const;  // c.f. std::filesystem::path::u8string()

public:
    EncodedStringView() = default;
    ~EncodedStringView() = default;
    EncodedStringView(const EncodedStringView&) = default;
    EncodedStringView& operator=(const EncodedStringView&) = default;
    EncodedStringView(EncodedStringView&&) = default;
    EncodedStringView& operator=(EncodedStringView&&) = default;

    // Need the const char* overloads to avoid creating temporary std::basic_string<> instances.
    // Routnes always return a copy, never a reference, so there's no additional overhead
    // with storing a raw pointer rather than a pointer to  std::basic_string<>.
    EncodedStringView(coda_oss::u8string::const_pointer);
    EncodedStringView(const coda_oss::u8string&);
    EncodedStringView(str::W1252string::const_pointer);
    EncodedStringView(const str::W1252string&);

    // Don't want to make it easy to use these; a known encoding is preferred.
    explicit EncodedStringView(std::string::const_pointer);  // Assume platform native encoding: UTF-8 on Linux, Windows-1252 on Windows
    explicit EncodedStringView(const std::string&);  // Assume platform native encoding: UTF-8 on Linux, Windows-1252 on Windows

    // Can't "view" UTF-16 or UTF-32 data; we assume we're looking at an 8-bit encoding,
    // either UTF-8 or Windows-1252.

    // Input is encoded as specified on all platforms.
    template <typename TBasicString>
    static EncodedStringView create(const char* s)
    {
        using const_pointer = typename TBasicString::const_pointer;
        return EncodedStringView(str::cast<const_pointer>(s));
    }
    template <typename TBasicString>
    static EncodedStringView create(const std::string& s)
    {
        return create<TBasicString>(s.c_str());
    }
    static EncodedStringView fromUtf8(const char* s)
    {
        return create<coda_oss::u8string>(s);
    }
    static EncodedStringView fromUtf8(const std::string& s)
    {
        return create<coda_oss::u8string>(s);
    }
    static EncodedStringView fromWindows1252(const char* s)
    {
        return create<str::W1252string>(s);
    }
    static EncodedStringView fromWindows1252(const std::string& s)
    {
        return create<str::W1252string>(s);
    }

    // Regardless of what string we're looking at, return a string in platform
    // native encoding: UTF-8 on Linux, Windows-1252 on Windows; this
    // might result in string conversion.
    std::string native() const; // c.f. std::filesystem::path::native()

    // Convert (perhaps) whatever we're looking at to UTF-8
    coda_oss::u8string u8string() const;  // c.f. std::filesystem::path::u8string()
    std::string& toUtf8(std::string&) const; // std::string is encoded as UTF-8, always.

    // Convert whatever we're looking at to UTF-16 or UTF-32
    std::u16string u16string() const;  // c.f. std::filesystem::path::u8string()
    std::u32string u32string() const;  // c.f. std::filesystem::path::u8string()
    // This is especially useful on Windows because the default for characters
    // is WCHAR (char* is converted to UTF-16).
    std::wstring wstring() const; // UTF-16 on Windows, UTF-32 on Linux

    // With some older C++ compilers, uint16_t may be used instead of char16_t :-(
    // Using this routine can avoid an extra copy.
    str::ui16string ui16string_() const; // use sparingly!

    bool operator_eq(const EncodedStringView&) const;

    struct details final
    {
        // Convert (perhaps) whatever we're looking at to Windows-1252
        // Intended for unit-testing; normal use is native().
        static str::W1252string w1252string(const EncodedStringView& v)
        {
            return v.w1252string();
        }
    };
};

inline bool operator==(const EncodedStringView& lhs, const EncodedStringView& rhs)
{
    return lhs.operator_eq(rhs);
}
inline bool operator!=(const EncodedStringView& lhs, const EncodedStringView& rhs)
{
    return !(lhs == rhs);
}

inline std::ostream& operator<<(std::ostream& os, const EncodedStringView& esv)
{
    os << esv.native();
    return os;
}

}
#endif  // CODA_OSS_str_EncodedStringView_h_INCLLUDED_

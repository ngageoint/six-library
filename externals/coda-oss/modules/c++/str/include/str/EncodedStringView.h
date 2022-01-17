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
#include <memory>

#include "coda_oss/span.h"
 #include "str/Encoding.h"

/*!
 * \file EncodedStringView.h
 * \brief A String that can be either UTF-8 or "native" 
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
class EncodedStringView final
{
    // Since we only support two encodings--UTF-8 (native on Linux) and Windows-1252
    // (native on Windows)--both of which are 8-bits, a simple "bool" flag will do.
    coda_oss::span<const char> mString;
    static constexpr bool mNativeIsUtf8 = details::Platform == details::PlatformType::Linux ? true : false;
    bool mIsUtf8 = mNativeIsUtf8;

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
    EncodedStringView(sys::U8string::const_pointer);
    EncodedStringView(const sys::U8string&);
    EncodedStringView(str::W1252string::const_pointer);
    EncodedStringView(const str::W1252string&);

    // Don't want to make it easy to use these; a known encoding is preferred.
    explicit EncodedStringView(std::string::const_pointer);  // Assume platform native encoding: UTF-8 on Linux, Windows-1252 on Windows
    explicit EncodedStringView(const std::string&);  // Assume platform native encoding: UTF-8 on Linux, Windows-1252 on Windows
    
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

    // Regardless of what string we're looking at, return a string in platform
    // native encoding: UTF-8 on Linux, Windows-1252 on Windows; this
    // might result in string conversion.
    std::string native() const; // c.f. std::filesystem::path::native()

    // Convert (perhaps) whatever we're looking at to UTF-8
    sys::U8string to_u8string() const;
    std::string& toUtf8(std::string&) const; // std::string is encoded as UTF-8, always.

    // Only casting done, no conversion.  This should be OK as all three
    // string types are 8-bit encodings.
    //
    // Intentionally a bit of a mouth-full as these routines should be used sparingly.
    template <typename TConstPointer>
    TConstPointer cast() const;  // returns NULL if stored pointer not of the desired type

    bool operator_eq(const EncodedStringView&) const;
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

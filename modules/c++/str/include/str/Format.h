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


#pragma once
#ifndef CODA_OSS_str_Format_h_INCLUDED_
#define CODA_OSS_str_Format_h_INCLUDED_

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

#include <string>

#include "config/Exports.h"

namespace str
{
namespace details
{
CODA_OSS_API std::string Format_(const char* format, ...);

inline auto Format(const char* format)
{
    return Format_(format);
}

template<typename T1>
inline auto Format(const char* format, const T1& t1)
{
    return Format_(format, t1);
}
template <typename T1, typename T2>
inline auto Format(const char* format, const T1& t1, const T2& t2)
{
    return Format_(format, t1, t2);
}
template <typename T1, typename T2, typename T3>
inline auto Format(const char* format, const T1& t1, const T2& t2, const T3& t3)
{
    return Format_(format, t1, t2, t3);
}
template <typename T1, typename T2, typename T3, typename T4>
inline auto Format(const char* format, const T1& t1, const T2& t2, const T3& t3, const T4& t4)
{
    return Format_(format, t1, t2, t3, t4);
}
}


/*!
 *  \param format  The format
 *  \param ... Any printf like thing
 */

inline auto Format(const char* format)
{
    return details::Format(format);
}

inline auto Format(const char* format, const char* pStr)
{
    return details::Format(format, pStr);
}
inline auto Format(const char* format, const std::string& s)
{
    return Format(format, s.c_str());
}
inline auto Format(const char* format, int i)
{
    return details::Format(format, i);
}
inline auto Format(const char* format, uint32_t i)
{
    return details::Format(format, i);
}
inline auto Format(const char* format, ptrdiff_t l)
{
    return details::Format(format, l);
}
inline auto Format(const char* format, size_t ul)
{
    return details::Format(format, ul);
}
inline auto Format(const char* format, float f)
{
    return details::Format(format, f);
}
inline auto Format(const char* format, double d)
{
    return details::Format(format, d);
}
inline auto Format(const char* format, const void* p)
{
    return details::Format(format, p);
}

inline auto Format(const char* format, char ch, const char* pStr)
{
    return details::Format(format, ch, pStr);
}
inline auto Format(const char* format, char ch, const std::string& s)
{
    return Format(format, ch, s.c_str());
}
inline auto Format(const char* format, const std::string& s1, const std::string& s2)
{
    return details::Format(format, s1.c_str(), s2.c_str());
}
inline auto Format(const char* format, const std::string& s, size_t ul)
{
    return details::Format(format, s.c_str(), ul);
}
inline auto Format(const char* format, char ch1, char ch2)
{
    return details::Format(format, ch1, ch2);
}
inline auto Format(const char* format, int i1, int i2)
{
    return details::Format(format, i1, i2);
}
inline auto Format(const char* format, long l1, long l2)
{
    return details::Format(format, l1, l2);
}
inline auto Format(const char* format, uint32_t ui1, uint32_t ui2)
{
    return details::Format(format, ui1, ui2);
}
inline auto Format(const char* format, size_t ul1, size_t ul2)
{
    return details::Format(format, ul1, ul2);
}
inline auto Format(const char* format, size_t ul1, int i2)
{
    return details::Format(format, ul1, i2);
}
inline auto Format(const char* format, double d1, double d2)
{
    return details::Format(format, d1, d2);
}
inline auto Format(const char* format, int i, const char* pStr)
{
    return details::Format(format, i, pStr);
}
inline auto Format(const char* fmt, int i, const std::string& s)
{
    return Format(fmt, i, s.c_str());
}

inline auto Format(const char* format, char ch1, char ch2, const std::string& s)
{
    return details::Format(format, ch1, ch2, s.c_str());
}
inline auto Format(const char* format, int i1, int i2, unsigned long ul)
{
    return details::Format(format, i1, i2, ul);
}
inline auto Format(const char* format, int i1, int i2, int i3)
{
    return details::Format(format, i1, i2, i3);
}
inline auto Format(const char* format, float f1, float f2, float f3)
{
    return details::Format(format, f1, f2, f3);
}
inline std::string Format(const char* format, const std::string& s1, int i2, int i3)
{
    return details::Format(format, s1.c_str(), i2, i3);
}
inline auto Format(const char* format, const std::string& s1, const std::string& s2, uint32_t ui)
{
    return details::Format(format, s1.c_str(), s2.c_str(), ui);
}
inline auto Format(const char* format, const std::string& s1, const std::string& s2, const std::string& s3)
{
    return details::Format(format, s1.c_str(), s2.c_str(), s3.c_str());
}

inline auto Format(const char* format, int i1, int i2, int i3, int i4)
{
    return details::Format(format, i1, i2, i3, i4);
}
inline auto Format(const char* format, uint32_t ui1, uint32_t ui2, uint32_t ui3, uint32_t ui4)
{
    return details::Format(format, ui1, ui2, ui3, ui4);
}
inline auto Format(const char* format, const char* pStr1, const std::string& s2, const char* pStr3, const std::string& s4)
{
    return details::Format(format, pStr1, s2.c_str(), pStr3, s4.c_str());
}
inline auto Format(const char* format, const std::string& s1, int i2, const std::string& s3, int i4)
{
    return details::Format(format, s1.c_str(), i2, s3.c_str(), i4);
}
}

#define FmtX(format, ...) str::Format(format, __VA_ARGS__)

#endif  // CODA_OSS_str_Format_h_INCLUDED_

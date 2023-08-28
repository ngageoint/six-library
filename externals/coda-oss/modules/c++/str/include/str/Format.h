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
#include<stdint.h>
#include<stddef.h>

#include <string>

#include "config/Exports.h"

namespace str
{
struct CODA_OSS_API Format final
{
    Format(const char* format, ...);

    operator std::string() const
    {
        return mString;
    }

private:
    std::string mString;
};

/*!
 *  \param format  The format
 *  \param ... Any printf like thing
 */
//CODA_OSS_API std::string format(const char* format, ...);

inline std::string format(const char* format)
{
    return Format(format);
}

inline std::string format(const char* format, const char* pStr)
{
    return Format(format, pStr);
}
inline std::string format(const char* format, const std::string& s)
{
    return Format(format, s.c_str());
}
inline std::string format(const char* format, int i)
{
    return Format(format, i);
}
inline std::string format(const char* format, uint32_t i)
{
    return Format(format, i);
}
inline std::string format(const char* format, ptrdiff_t l)
{
    return Format(format, l);
}
inline std::string format(const char* format, size_t ul)
{
    return Format(format, ul);
}
inline std::string format(const char* format, float f)
{
    return Format(format, f);
}

inline std::string format(const char* format, char ch, const char* pStr)
{
    return Format(format, ch, pStr);
}
inline std::string format(const char* format, char ch, const std::string& s)
{
    return Format(format, ch, s.c_str());
}
inline std::string format(const char* format, const std::string& s1, const std::string& s2)
{
    return Format(format, s1.c_str(), s2.c_str());
}
inline std::string format(const char* format, const std::string& s, size_t ul)
{
    return Format(format, s.c_str(), ul);
}
inline std::string format(const char* format, char ch1, char ch2)
{
    return Format(format, ch1, ch2);
}
inline std::string format(const char* format, size_t ul1, size_t ul2)
{
    return Format(format, ul1, ul2);
}
inline std::string format(const char* format, int i, const std::string& s)
{
    return Format(format, i, s.c_str());
}
inline std::string format(const char* format, int i, const char* pStr)
{
    return Format(format, i, pStr);
}

inline std::string format(const char* format, char ch1, char ch2, const std::string& s)
{
    return Format(format, ch1, ch2, s.c_str());
}
inline std::string format(const char* format, int i1, int i2, unsigned long ul)
{
    return Format(format, i1, i2, ul);
}
inline std::string format(const char* format, const std::string& s1, const std::string& s2, uint32_t ui)
{
    return Format(format, s1.c_str(), s2.c_str(), ui);
}

}
#endif  // CODA_OSS_str_Format_h_INCLUDED_
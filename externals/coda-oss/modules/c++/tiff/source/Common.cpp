/* =========================================================================
 * This file is part of tiff-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * tiff-c++ is free software; you can redistribute it and/or modify
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

#include "tiff/Common.h"

#include <sstream>
#include <string.h>

//! Initialize the byte count values for each TIFF type.
short tiff::Const::mTypeSizes[tiff::Const::Type::MAX] =
{ 0, 1, 1, 2, 4, 8, 1, 1, 2, 4, 8, 4, 8 };

std::string tiff::RationalPrintStrategy::toString(const sys::Uint32_T data)
{
    std::ostringstream tempStream;
    sys::Uint32_T numerator = *((sys::Uint32_T *)(&data));
    sys::Uint32_T denominator = *((sys::Uint32_T *)(&data + sizeof(sys::Uint32_T)));
    tempStream << numerator << "/" << denominator;

    return tempStream.str();
}
std::string tiff::RationalPrintStrategy::toString(const sys::Uint64_T data)
{
    return toString(static_cast<sys::Uint32_T>(data));
}

template<typename T>
static inline void memcpy_(T* pDest, const void* pSrc)
{
    memcpy(pDest, pSrc, sizeof(T));
}

sys::Uint64_T tiff::combine(sys::Uint32_T numerator,
        sys::Uint32_T denominator)
{
    sys::Uint64_T value;

    //sys::Uint32_T *ptr = (sys::Uint32_T *)&value;
    auto ptr = reinterpret_cast<sys::ubyte*>(&value);  // TODO: std::byte // reinterpret_cast<> to std::byte is allowed by the standard
    memcpy_(ptr, &numerator);
    memcpy_(ptr + sizeof(sys::Uint32_T), &denominator);

    return value;
}
void tiff::split(sys::Uint64_T value, sys::Uint32_T &numerator,
                 sys::Uint32_T &denominator)
{
    // Using casts generate a warning: dereferencing type-punned pointer will break strict-aliasing rules[-Wstrict-aliasing]
    // Do the reverse of combine() and use memcpy().
    //numerator = ((sys::Uint32_T*)&value)[0];
    //denominator = ((sys::Uint32_T*)&value)[1];
    auto ptr = reinterpret_cast<const sys::ubyte*>(&value);  // TODO: std::byte // reinterpret_cast<> to std::byte is allowed by the standard
    memcpy_(&numerator, ptr);
    memcpy_(&denominator, ptr + sizeof(sys::Uint32_T));
}


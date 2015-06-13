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

#include "tiff/TypeFactory.h"
#include "tiff/Common.h"
#include "tiff/GenericType.h"
#include <import/except.h>

tiff::TypeInterface *tiff::TypeFactory::create(const unsigned char *data,
        const unsigned short type)
{
    tiff::TypeInterface *tiffType = NULL;
    switch (type)
    {
    case tiff::Const::Type::BYTE:
        tiffType = new tiff::GenericType<unsigned char>(data);
        break;
    case tiff::Const::Type::ASCII:
        tiffType = new tiff::GenericType<char>(data);
        break;
    case tiff::Const::Type::SHORT:
        tiffType = new tiff::GenericType<unsigned short>(data);
        break;
    case tiff::Const::Type::LONG:
        tiffType = new tiff::GenericType<sys::Uint32_T>(data);
        break;
    case tiff::Const::Type::RATIONAL:
        tiffType  = new tiff::GenericType<sys::Uint64_T,
            tiff::RationalPrintStrategy>(data);
        break;
    case tiff::Const::Type::SBYTE:
        tiffType = new tiff::GenericType<char>(data);
        break;
    case tiff::Const::Type::UNDEFINED:
        tiffType = new tiff::GenericType<unsigned char>(data);
        break;
    case tiff::Const::Type::SSHORT:
        tiffType = new tiff::GenericType<short>(data);
        break;
    case tiff::Const::Type::SLONG:
        tiffType = new tiff::GenericType<sys::Uint32_T>(data);
        break;
    case tiff::Const::Type::SRATIONAL:
        tiffType = new tiff::GenericType<sys::Uint64_T,
                tiff::RationalPrintStrategy>(data);
        break;
    case tiff::Const::Type::FLOAT:
        tiffType = new tiff::GenericType<float>(data);
        break;
    case tiff::Const::Type::DOUBLE:
        tiffType = new tiff::GenericType<double>(data);
        break;
    default:
        throw except::Exception(Ctxt("Unsupported Type"));
    }
    return tiffType;
}

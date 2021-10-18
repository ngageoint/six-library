/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
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

#include <gsl/gsl.h>

#include "six/Data.h"

using namespace six;

/*!
 *  Num bytes per pixel can be determined also by the pixel type
 */
size_t Data::getNumBytesPerPixel() const
{
    const auto pixelType = getPixelType();
    switch (pixelType)
    {
    case PixelType::RE32F_IM32F:
    case PixelType::RE16I_IM16I:
    case PixelType::AMP8I_PHS8I:
    case PixelType::MONO8I:
    case PixelType::MONO8LU:
    case PixelType::MONO16I:
    case PixelType::RGB8LU:
    case PixelType::RGB24I:
        return gsl::narrow<size_t>(six::Constants::getNumBytesPerPixel(pixelType));

    default:
        throw except::Exception(Ctxt("Cannot determine number of bytes per pixel - invalid or unsupported pixel type"));
    }
}

size_t Data::getNumChannels() const
{
    switch (getPixelType())
    {
    case PixelType::RE32F_IM32F:
    case PixelType::RE16I_IM16I:
    case PixelType::AMP8I_PHS8I:
        return 2;
    case PixelType::RGB24I:
        return 3;
    case PixelType::MONO16I:
    case PixelType::MONO8I:
    case PixelType::MONO8LU:
    case PixelType::RGB8LU:
        return 1;
    default:
        throw except::Exception(Ctxt("Cannot determine number of channels - invalid or unsupported pixel type"));
    }
}

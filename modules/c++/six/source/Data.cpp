/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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
#include "six/Data.h"

using namespace six;

/*!
 *  Num bytes per pixel can be determined also by the pixel type
 */
unsigned long Data::getNumBytesPerPixel() const
{
    int nbpp = 1;
    PixelType pixelType = getPixelType();
    switch (pixelType)
    {
    case PixelType::RE32F_IM32F:
        nbpp = 8;
        break;

    case PixelType::RE16I_IM16I:
        nbpp = 4;
        break;
    case PixelType::RGB24I:
        nbpp = 3;
        break;
    default:
        break;
    }
    return nbpp;
}

unsigned long Data::getNumChannels() const
{
    long numChannels = 1;
    PixelType pixelType = getPixelType();
    switch (pixelType)
    {
    case PixelType::RE32F_IM32F:
    case PixelType::RE16I_IM16I:
        numChannels = 2;
        break;
    case PixelType::RGB24I:
        numChannels = 3;
        break;
    default:
        break;
    }
    return numChannels;
}

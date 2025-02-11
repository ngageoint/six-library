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

#ifndef __TIFF_UTILS_H__
#define __TIFF_UTILS_H__

#include "tiff/IFD.h"

namespace tiff
{

class Utils
{
public:

    static bool hasGeoTiffIFD(const tiff::IFD* inputIFD);
    static tiff::IFD* createGeoTiffIFD(tiff::IFD* inputIFD);

private:
    Utils()
    {
    }

};

}
#endif // __TIFF_UTILS_H__

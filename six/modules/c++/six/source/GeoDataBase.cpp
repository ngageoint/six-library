/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2020, MDA Information Systems LLC
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
#include <memory>

#include "six/GeoDataBase.h"

namespace six
{
// const double GeoDataBase::ECF_THRESHOLD = 1e-2;

bool GeoDataBase::operator==(const GeoDataBase& rhs) const
{
    return (earthModel == rhs.earthModel &&
            imageCorners == rhs.imageCorners && validData == rhs.validData
            && geoInfos == rhs.geoInfos);
}
}


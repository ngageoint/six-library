/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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
#include <six/CollectionInformation.h>

namespace six
{
CollectionInformation::CollectionInformation()
{
    collectorName = Init::undefined<std::string>();
    collectType = Init::undefined<six::CollectType>();
    illuminatorName = Init::undefined<std::string>();
    coreName = Init::undefined<std::string>();
    radarMode = Init::undefined<RadarModeType>();
    radarModeID = Init::undefined<std::string>();
    releaseInfo = Init::undefined<std::string>();
    mClassification = Init::undefined<std::string>();
}

CollectionInformation* CollectionInformation::clone() const
{
    return new CollectionInformation(*this);
}
}

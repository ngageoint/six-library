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

std::ostream& operator<< (std::ostream& os, const six::CollectionInformation& c)
{
    os << "CollectionID:: \n"
        << "  CollectorName    : " << c.collectorName << "\n";
    if (!six::Init::isUndefined(c.illuminatorName))
    {
        os << "  IlluminatorName  : " << c.illuminatorName << "\n";
    }
    os << "  CoreName         : " << c.coreName << "\n";
    if (!six::Init::isUndefined(c.collectType))
    {
        os << "  CollectType      : " << c.collectType << "\n";
    }
    os << "  RadarMode        : " << c.radarMode << "\n";
    if (!six::Init::isUndefined(c.radarModeID))
    {
        os << "  RadarModeID      : " << c.radarModeID << "\n";
    }
    if (!six::Init::isUndefined(c.releaseInfo))
    {
        os << "  ReleaseInfo      : " << c.releaseInfo << "\n";
    }
    for (size_t ii = 0; ii < c.countryCodes.size(); ++ii)
    {
        os << "  CountryCodes     : " << c.countryCodes[ii] << "\n";
    }
    for (size_t ii = 0; ii < c.parameters.size(); ++ii)
    {
        os << "  Parameter name   : " << c.parameters[ii].getName() << "\n"
            << "  Parameter value  : " << c.parameters[ii].str() << "\n";
    }
    return os;
}
}

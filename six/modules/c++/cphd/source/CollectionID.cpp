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
#include <cphd/CollectionID.h>

namespace cphd
{
CollectionID::CollectionID()
{
    collectorName = six::Init::undefined<std::string>();
    collectType = six::Init::undefined<six::CollectType>();
    illuminatorName = six::Init::undefined<std::string>();
    coreName = six::Init::undefined<std::string>();
    radarMode = six::Init::undefined<six::RadarModeType>();
    radarModeID = six::Init::undefined<std::string>();
    releaseInfo = six::Init::undefined<std::string>();
    mClassification = six::Init::undefined<std::string>();
}

CollectionID* CollectionID::clone() const
{
    return new CollectionID(*this);
}

std::ostream& operator<< (std::ostream& os, const CollectionID& c)
{
    os << "CollectionID:: \n"
        << "  CollectorName    : " << c.collectorName << "\n"
        << "  IlluminatorName  : " << c.illuminatorName << "\n"
        << "  CoreName         : " << c.coreName << "\n"
        << "  CollectType      : " << c.collectType << "\n"
        << "  RadarMode        : " << c.radarMode << "\n"
        << "  RadarModeID      : " << c.radarMode << "\n"
        << "  ReleaseInfo      : " << c.releaseInfo << "\n";
    for (size_t i = 0; i < c.countryCodes.size(); ++i)
    {
        os << "  CountryCodes     : " << c.countryCodes[i] << "\n";
    }
    for (size_t i = 0; i < c.parameters.size(); ++i)
    {
        os << "  Parameter name   : " << c.parameters[i].getName() << "\n"
            << "  Parameter value  : " << c.parameters[i].str() << "\n";
    }
    return os;
}

}

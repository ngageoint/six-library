/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * cphd-c++ is free software; you can redistribute it and/or modify
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
    releaseInfo = six::Init::undefined<std::string>();
}

std::ostream& operator<< (std::ostream& os, const CollectionID& c)
{
    os << "CollectionID:: \n"
        << "  CollectorName    : " << c.collectInfo.collectorName << "\n";
    if (!six::Init::isUndefined(c.collectInfo.illuminatorName))
    {
        os << "  IlluminatorName  : " << c.collectInfo.illuminatorName << "\n";
    }
    os << "  CoreName         : " << c.collectInfo.coreName << "\n"
        << "  CollectType      : " << c.collectInfo.collectType << "\n"
        << "  RadarMode        : " << c.collectInfo.radarMode << "\n"
        << "  RadarModeID      : " << c.collectInfo.radarMode << "\n"
        << "  ReleaseInfo      : " << c.releaseInfo << "\n";
    for (size_t ii = 0; ii < c.collectInfo.countryCodes.size(); ++ii)
    {
        os << "  CountryCodes     : " << c.collectInfo.countryCodes[ii] << "\n";
    }
    for (size_t ii = 0; ii < c.collectInfo.parameters.size(); ++ii)
    {
        os << "  Parameter name   : " << c.collectInfo.parameters[ii].getName() << "\n"
            << "  Parameter value  : " << c.collectInfo.parameters[ii].str() << "\n";
    }
    return os;
}

}

/* =========================================================================
 * This file is part of cphd03-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * cphd03-c++ is free software; you can redistribute it and/or modify
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

#include <ostream>

#include <cphd03/Utilities.h>

namespace cphd03
{
size_t getNumBytesPerSample(cphd::SampleType sampleType)
{
    switch (sampleType)
    {
        case cphd::SampleType::RE08I_IM08I:
            return 2;
        case cphd::SampleType::RE16I_IM16I:
            return 4;
        case cphd::SampleType::RE32F_IM32F:
            return 8;
        default:
            return 0;
    }
}

template<> std::string toString(const cphd::CollectionInformation& ci)
{
    std::ostringstream os;

    os << "CollectionInformation::" << "\n";
    os << "  collectorName  : " << ci.collectorName << "\n";

    if (!six::Init::isUndefined(ci.illuminatorName))
    {
        os << "  illuminatorName: " << ci.illuminatorName << "\n";
    }

    os << "  coreName       : " << ci.coreName << "\n";

    if (ci.collectType != cphd::CollectType::NOT_SET )
    {
        os << "  collectType    : " << ci.collectType << "\n";
    }

    os << "  radarMode      : " << ci.radarMode << "\n";

    if (!six::Init::isUndefined(ci.radarModeID))
    {
        os << "  radarModeID    : " << ci.radarModeID << "\n";
    }

    os << "  classification : " << ci.getClassificationLevel() << "\n";

    for (size_t ii = 0; ii < ci.countryCodes.size(); ++ii)
    {
        os << "  Country Code   : " << ci.countryCodes[ii] << "\n";
    }

    return os.str();
}
}

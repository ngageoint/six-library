/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2013, General Dynamics - Advanced Information Systems
 *
 * six.sidd-c++ is free software; you can redistribute it and/or modify
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
#include "six/sidd/GeographicAndTarget.h"

using namespace six;
using namespace six::sidd;

TargetInformation* TargetInformation::clone() const
{
    return new TargetInformation(*this);
}

GeographicInformation* GeographicInformation::clone() const
{
    return new GeographicInformation(*this);
}

GeographicCoverage::GeographicCoverage(RegionType rt) :
    regionType(rt)
{
    //! TODO: add one to all one or more types
    if (regionType == RegionType::GEOGRAPHIC_INFO)
        geographicInformation.reset(new GeographicInformation());
}

GeographicCoverage* GeographicCoverage::clone() const
{
    return new GeographicCoverage(*this);
}

GeographicAndTarget* GeographicAndTarget::clone() const
{
    return new GeographicAndTarget(*this);
}


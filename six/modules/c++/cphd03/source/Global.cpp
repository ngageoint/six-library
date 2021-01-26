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

#include <six/Init.h>
#include <six/Utilities.h>
#include <cphd03/Utilities.h>
#include <cphd03/Global.h>

namespace cphd03
{
DwellTimeParameters::DwellTimeParameters() :
    codTimePoly(six::Init::undefined<cphd::Poly2D>()),
    dwellTimePoly(six::Init::undefined<cphd::Poly2D>())
{
}

std::ostream& operator<< (std::ostream& os, const DwellTimeParameters& d)
{
    os << "DwellTimeParameters::\n"
       << "  CODTimePoly  :\n" << d.codTimePoly << "\n"
       << "  DwellTimePoly:\n" << d.dwellTimePoly << "\n";
    return os;
}

AreaPlane::AreaPlane() :
    referencePoint(six::Init::undefined<six::ReferencePoint>())
{
}

bool AreaPlane::operator==(const AreaPlane& other) const
{
    if (referencePoint != other.referencePoint &&
        xDirection != other.xDirection &&
        yDirection != other.yDirection)
    {
        return false;
    }

    if (dwellTime.get() && other.dwellTime.get())
    {
        if (*dwellTime != *other.dwellTime)
        {
            return false;
        }
    }
    else if (dwellTime.get() || other.dwellTime.get())
    {
        return false;
    }

    return true;
}

std::ostream& operator<< (std::ostream& os, const AreaPlane& d)
{
    os << "AreaPlane::\n"
       << "  xDirection: " << d.xDirection << "\n"
       << "  yDirection: " << d.yDirection << "\n";
    if (d.dwellTime.get())
    {
        os << "  dwellTime : " << *d.dwellTime << "\n";
    }
    return os;
}

ImageArea::ImageArea() :
    acpCorners(six::Init::undefined<cphd::LatLonAltCorners>())
{
}

bool ImageArea::operator==(const ImageArea& other) const
{
    for (size_t ii = 0; ii < cphd::LatLonAltCorners::NUM_CORNERS; ++ii)\
    {
        if (!(acpCorners.getCorner(ii) == other.acpCorners.getCorner(ii)))
        {
           return false;
        }
    }

    if (plane.get() && other.plane.get())
    {
        if (*plane.get() != *other.plane.get())
        {
            return false;
        }
    }
    else if (plane.get() || other.plane.get())
    {
        return false;
    }

    return true;
}

std::ostream& operator<< (std::ostream& os, const ImageArea& d)
{
    os << "ImageArea::\n";
    if (d.plane.get())
    {
        os << "  plane     : " <<  *d.plane << "\n";
    }

    return os;
}

Global::Global() :
    refFrequencyIndex(six::Init::undefined<size_t>()),
    collectDuration(0.0),
    txTime1(0.0),
    txTime2(0.0)
{
}

std::ostream& operator<< (std::ostream& os, const Global& d)
{
    os << "Global::\n"
       << "  domainType       : " << d.domainType << "\n"
       << "  phaseSGN         : " << d.phaseSGN << "\n";

    if (!six::Init::isUndefined(d.refFrequencyIndex))
    {
        os << "  refFrequencyIndex: " << d.refFrequencyIndex << "\n";
    }

    os << "  collectStart     : " << six::toString(d.collectStart)
       << "\n"
       << "  collectDuration  : " << d.collectDuration << "\n"
       << "  txTime1          : " << d.txTime1 << "\n"
       << "  txTime2          : " << d.txTime2 << "\n"
       << "  imageArea        : " << d.imageArea << "\n";
    return os;
}
}

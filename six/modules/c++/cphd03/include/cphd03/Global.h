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

#pragma once
#ifndef __CPHD03_GLOBAL_H__
#define __CPHD03_GLOBAL_H__

#include <ostream>

#include <mem/ScopedCopyablePtr.h>
#include <six/sicd/RadarCollection.h>
#include <cphd/Enums.h>
#include <cphd/Types.h>

#include "cphd03/Exports.h"

namespace cphd03
{
// Optional segment DwellTime
struct SIX_CPHD03_API DwellTimeParameters
{
    DwellTimeParameters();

    bool operator==(const DwellTimeParameters& other) const
    {
        return codTimePoly == other.codTimePoly &&
               dwellTimePoly == other.dwellTimePoly;
    }

    bool operator!=(const DwellTimeParameters& other) const
    {
        return !((*this) == other);
    }

    cphd::Poly2D codTimePoly;
    cphd::Poly2D dwellTimePoly;
};

std::ostream& operator<< (std::ostream& os, const DwellTimeParameters& d);

typedef six::sicd::AreaDirectionParameters AreaDirectionParameters;

struct SIX_CPHD03_API AreaPlane
{
    AreaPlane();

    bool operator==(const AreaPlane& other) const;

    bool operator!=(const AreaPlane& other) const
    {
        return !((*this) == other);
    }

    six::ReferencePoint referencePoint;
    AreaDirectionParameters xDirection;
    AreaDirectionParameters yDirection;
    mem::ScopedCopyablePtr<DwellTimeParameters> dwellTime;
};

std::ostream& operator<< (std::ostream& os, const AreaPlane& d);

struct SIX_CPHD03_API ImageArea
{
    ImageArea();

    bool operator==(const ImageArea& other) const;

    bool operator!=(const ImageArea& other) const
    {
        return !((*this) == other);
    }

    cphd::LatLonAltCorners acpCorners;
    mem::ScopedCopyablePtr<AreaPlane> plane;
};

std::ostream& operator<< (std::ostream& os, const ImageArea& d);

struct SIX_CPHD03_API Global final
{
    Global();

    bool operator==(const Global& other) const
    {
        return domainType == other.domainType &&
               phaseSGN == other.phaseSGN &&
               refFrequencyIndex == other.refFrequencyIndex &&
               collectStart == other.collectStart &&
               collectDuration == other.collectDuration &&
               txTime1 == other.txTime1 &&
               txTime2 == other.txTime2 &&
               imageArea == other.imageArea;
    }

    bool operator !=(const Global& other) const
    {
        return !((*this) == other);
    }

    cphd::DomainType domainType;
    cphd::PhaseSGN   phaseSGN;

    //! Indicates the RF freq values expressed as offsets from a ref freq
    size_t refFrequencyIndex;

    //! Collection date/time UTC, measured from collection start
    cphd::DateTime collectStart;

    //! Duration of collection period
    double collectDuration;

    double txTime1;
    double txTime2;

    ImageArea imageArea;
};

std::ostream& operator<< (std::ostream& os, const Global& d);
}

#endif

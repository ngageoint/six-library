/******************************************************************************
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * Developed in the performance of one or more U.S. Government contracts.
 * The U.S. Government has Unlimited Rights in this computer software as set
 * forth in the Rights in Technical Data and Compute Software: Noncommercial
 * Items clauses.
 ******************************************************************************/


#ifndef __CPHD_GLOBAL_H__
#define __CPHD_GLOBAL_H__

#include <ostream>

#include <mem/ScopedCopyablePtr.h>
#include <six/sicd/RadarCollection.h>
#include <cphd/Enums.h>
#include <cphd/Types.h>

namespace cphd
{
// Optional segment DwellTime
struct DwellTimeParameters
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

    Poly2D codTimePoly;
    Poly2D dwellTimePoly;
};

std::ostream& operator<< (std::ostream& os, const DwellTimeParameters& d);

typedef six::sicd::AreaDirectionParameters AreaDirectionParameters;

struct AreaPlane
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

struct ImageArea
{
    ImageArea();

    bool operator==(const ImageArea& other) const;

    bool operator!=(const ImageArea& other) const
    {
        return !((*this) == other);
    }

    LatLonAltCorners acpCorners;
    mem::ScopedCopyablePtr<AreaPlane> plane;
};

std::ostream& operator<< (std::ostream& os, const ImageArea& d);

struct Global
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

    DomainType domainType;
    PhaseSGN   phaseSGN;

    //! Indicates the RF freq values expressed as offsets from a ref freq
    size_t refFrequencyIndex;

    //! Collection date/time UTC, measured from collection start
    DateTime collectStart;
    
    //! Duration of collection period
    double collectDuration;

    double txTime1;
    double txTime2;

    ImageArea imageArea;
};

std::ostream& operator<< (std::ostream& os, const Global& d);
}

#endif

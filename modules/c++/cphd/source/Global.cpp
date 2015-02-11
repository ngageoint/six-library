/******************************************************************************
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * Developed in the performance of one or more U.S. Government contracts.
 * The U.S. Government has Unlimited Rights in this computer software as set
 * forth in the Rights in Technical Data and Compute Software: Noncommercial
 * Items clauses.
 ******************************************************************************/

#include <six/Init.h>
#include <six/Utilities.h>
#include <cphd/Utilities.h>
#include <cphd/Global.h>

namespace cphd
{
DwellTimeParameters::DwellTimeParameters() :
    codTimePoly(six::Init::undefined<Poly2D>()),
    dwellTimePoly(six::Init::undefined<Poly2D>())
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
    acpCorners(six::Init::undefined<LatLonAltCorners>())
{
}

bool ImageArea::operator==(const ImageArea& other) const
{
    for (size_t ii = 0; ii < LatLonAltCorners::NUM_CORNERS; ++ii)\
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
       << "  domainType       : " << d.domainType.toString() << "\n"
       << "  phaseSGN         : " << d.phaseSGN.toString() << "\n";

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

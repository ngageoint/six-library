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
#include <six/sicd/Antenna.h>
#include <six/Utilities.h>

namespace six
{
namespace sicd
{
ElectricalBoresight::ElectricalBoresight() :
    dcxPoly(Init::undefined<Poly1D>()),
    dcyPoly(Init::undefined<Poly1D>())
{
}

std::ostream& operator<< (std::ostream& os, const ElectricalBoresight& d)
{
    os << "ElectricalBoresight::" << "\n"
       << "  dcxPoly: " << d.dcxPoly << "\n"
       << "  dcyPoly: " << d.dcyPoly << "\n";
    if (d.useEBPVP)
    {
        os << "  useEBPVP: " << *d.useEBPVP << "\n";
    }
    return os;
}

HalfPowerBeamwidths::HalfPowerBeamwidths() :
    dcx(Init::undefined<double>()),
    dcy(Init::undefined<double>())
{
}

std::ostream& operator<< (std::ostream& os, const HalfPowerBeamwidths& d)
{
    os << "HalfPowerBeamwidths::" << "\n"
       << "  dcx: " << d.dcx << "\n"
       << "  dcy: " << d.dcy << "\n";
    return os;
}

GainAndPhasePolys::GainAndPhasePolys() :
    gainPoly(Init::undefined<Poly2D>()),
    phasePoly(Init::undefined<Poly2D>())
{
}

std::ostream& operator<< (std::ostream& os, const GainAndPhasePolys& d)
{
    os << "  gainPoly:\n" << d.gainPoly << "\n"
       << "  phasePoly:\n" << d.phasePoly << "\n";
    if (d.antGPId)
    {
        os << "  antGPId:\n" << *d.antGPId << "\n";
    }
    return os;
}
AntennaParameters::AntennaParameters() : 
    xAxisPoly(Init::undefined<PolyXYZ>()),
    yAxisPoly(Init::undefined<PolyXYZ>()),
    frequencyZero(Init::undefined<double>()),
    gainBSPoly(Init::undefined<Poly1D>()),
    electricalBoresightFrequencyShift(Init::undefined<BooleanType>()),
    mainlobeFrequencyDilation(Init::undefined<BooleanType>())

{
}

bool AntennaParameters::operator==(const AntennaParameters& other) const
{
    if (xAxisPoly != other.xAxisPoly ||
        yAxisPoly != other.yAxisPoly ||
        frequencyZero != other.frequencyZero ||
        gainBSPoly != other.gainBSPoly ||
        electricalBoresightFrequencyShift !=
                other.electricalBoresightFrequencyShift ||
        mainlobeFrequencyDilation != other.mainlobeFrequencyDilation)
    {
        return false;
    }

    if (electricalBoresight.get() && other.electricalBoresight.get())
    {
        if (*electricalBoresight != *other.electricalBoresight)
        {
            return false;
        }
    }
    else if (electricalBoresight.get() || other.electricalBoresight.get())
    {
        return false;
    }

    if (halfPowerBeamwidths.get() && other.halfPowerBeamwidths.get())
    {
        if (*halfPowerBeamwidths != *other.halfPowerBeamwidths)
        {
            return false;
        }
    }
    else if (halfPowerBeamwidths.get() || other.halfPowerBeamwidths.get())
    {
        return false;
    }

    if (array.get() && other.array.get())
    {
        if (*array != *other.array)
        {
            return false;
        }
    }
    else if (array.get() || other.array.get())
    {
        return false;
    }

    if (element.get() && other.element.get())
    {
        if (*array != *other.array)
        {
            return false;
        }
    }
    else if (element.get() || other.element.get())
    {
        return false;
    }

    return true;
}

std::ostream& operator<< (std::ostream& os, const AntennaParameters& d)
{
    os << "  xAxisPoly:\n" << toString(d.xAxisPoly) << "\n"
       << "  yAxisPoly:\n" << toString(d.yAxisPoly) << "\n"
       << "  frequencyZero: " << d.frequencyZero << "\n";

    if (d.electricalBoresight.get())
    {
        os << "  " << *d.electricalBoresight << "\n";
    }

    if (d.halfPowerBeamwidths.get())
    {
        os << "  " << *d.halfPowerBeamwidths << "\n";
    }

    if (d.array.get())
    {
        os << "  Array:" << "\n"
           << *d.array << "\n";
    }

    if (d.element.get())
    {
        os << "  Elem:" << "\n"
           << *d.element << "\n";
    }

    if (!six::Init::isUndefined(d.gainBSPoly))
    {
       os << "  gainBSPoly   : " << d.gainBSPoly << "\n";
    }

    if (!six::Init::isUndefined(d.electricalBoresightFrequencyShift))
    {
        os << "  electricalBoresightFrequencyShift: "
                << d.electricalBoresightFrequencyShift << "\n";
    }

    if (!six::Init::isUndefined(d.mainlobeFrequencyDilation))
    {
        os << "  mainlobeFrequencyDilation        : "
           << d.mainlobeFrequencyDilation << "\n";
    }
     return os;
}
}
}

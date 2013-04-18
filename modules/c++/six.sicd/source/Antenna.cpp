/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2013, General Dynamics - Advanced Information Systems
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
#include "six/sicd/Antenna.h"

using namespace six;
using namespace six::sicd;

ElectricalBoresight::ElectricalBoresight() :
    dcxPoly(Init::undefined<Poly1D>()),
    dcyPoly(Init::undefined<Poly1D>())
{
}

HalfPowerBeamwidths::HalfPowerBeamwidths() :
    dcx(Init::undefined<double>()),
    dcy(Init::undefined<double>())
{
}

GainAndPhasePolys::GainAndPhasePolys() :
    gainPoly(Init::undefined<Poly2D>()),
    phasePoly(Init::undefined<Poly2D>())
{
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




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
#include "six/sicd/RMA.h"

using namespace six;
using namespace six::sicd;

RMAT::RMAT() :
    refTime(Init::undefined<double>()),
    refPos(Init::undefined<Vector3>()),
    refVel(Init::undefined<Vector3>()),
    distRefLinePoly(Init::undefined<Poly1D>()),
    cosDCACOAPoly(Init::undefined<Poly2D>()),
    kx1(Init::undefined<double>()),
    kx2(Init::undefined<double>()),
    ky1(Init::undefined<double>()),
    ky2(Init::undefined<double>()),
    dopConeAngleRef(Init::undefined<double>())
{
}

bool RMAT::operator==(const RMAT& rhs) const
{
    return (refTime == rhs.refTime &&
        refPos == rhs.refPos &&
        refVel == rhs.refVel &&
        distRefLinePoly == rhs.distRefLinePoly &&
        cosDCACOAPoly == rhs.cosDCACOAPoly &&
        kx1 == rhs.kx1 &&
        kx2 == rhs.kx2 &&
        ky1 == rhs.ky1 &&
        ky2 == rhs.ky2 &&
        dopConeAngleRef == rhs.dopConeAngleRef);
}

RMCR::RMCR() :
    refPos(Init::undefined<Vector3>()),
    refVel(Init::undefined<Vector3>()),
    dopConeAngleRef(Init::undefined<double>())
{
}

INCA::INCA() :
    timeCAPoly(Init::undefined<Poly1D>()),
    rangeCA(Init::undefined<double>()),
    freqZero(Init::undefined<double>()),
    dopplerRateScaleFactorPoly(Init::undefined<Poly2D>()),
    dopplerCentroidPoly(Init::undefined<Poly2D>()),
    dopplerCentroidCOA(Init::undefined<BooleanType>())
{
}

bool INCA::operator==(const INCA& rhs) const
{
    return (timeCAPoly == rhs.timeCAPoly &&
        rangeCA == rhs.rangeCA &&
        freqZero == rhs.freqZero &&
        dopplerRateScaleFactorPoly == rhs.dopplerRateScaleFactorPoly &&
        dopplerCentroidPoly == rhs.dopplerCentroidPoly &&
        dopplerCentroidCOA == rhs.dopplerCentroidCOA);
}

RMA::RMA() : 
    algoType(RMAlgoType::NOT_SET)
{
}


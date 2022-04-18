/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
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
#include "six/Radiometric.h"

using namespace six;

const char six::Radiometric::NL_ABSOLUTE[] = "ABSOLUTE";
const char six::Radiometric::NL_RELATIVE[] = "RELATIVE";

NoiseLevel::NoiseLevel() :
    noisePoly(Init::undefined<Poly2D>())
{
}

Radiometric::Radiometric() :
    rcsSFPoly(Init::undefined<Poly2D>()),
    betaZeroSFPoly(Init::undefined<Poly2D>()),
    sigmaZeroSFPoly(Init::undefined<Poly2D>()),
    sigmaZeroSFIncidenceMap(Init::undefined<AppliedType>()),
    gammaZeroSFPoly(Init::undefined<Poly2D>()),
    gammaZeroSFIncidenceMap(Init::undefined<AppliedType>())
{
}

bool Radiometric::operator==(const Radiometric& rhs) const
{
    return (noiseLevel == rhs.noiseLevel &&
        rcsSFPoly == rhs.rcsSFPoly &&
        betaZeroSFPoly == rhs.betaZeroSFPoly &&
        sigmaZeroSFPoly == rhs.sigmaZeroSFPoly &&
        sigmaZeroSFIncidenceMap == rhs.sigmaZeroSFIncidenceMap &&
        gammaZeroSFPoly == rhs.gammaZeroSFPoly &&
        gammaZeroSFIncidenceMap == rhs.gammaZeroSFIncidenceMap);
}
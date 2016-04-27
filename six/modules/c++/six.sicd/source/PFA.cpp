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
#include "six/sicd/PFA.h"

namespace six
{
namespace sicd
{
SlowTimeDeskew::SlowTimeDeskew() :
    applied(Init::undefined<BooleanType>())
{
}

PFA::PFA() :
    focusPlaneNormal(Init::undefined<Vector3>()),
    imagePlaneNormal(Init::undefined<Vector3>()),
    polarAngleRefTime(Init::undefined<double>()),
    krg1(Init::undefined<double>()),
    krg2(Init::undefined<double>()),
    kaz1(Init::undefined<double>()),
    kaz2(Init::undefined<double>())
{
}

bool PFA::operator==(const PFA& rhs) const
{
    return (focusPlaneNormal == rhs.focusPlaneNormal &&
        imagePlaneNormal == rhs.imagePlaneNormal &&
        polarAngleRefTime == rhs.polarAngleRefTime &&
        polarAnglePoly == rhs.polarAnglePoly &&
        spatialFrequencyScaleFactorPoly == rhs.spatialFrequencyScaleFactorPoly &&
        krg1 == rhs.krg1 &&
        krg2 == rhs.krg2 &&
        kaz1 == rhs.kaz2 &&
        kaz2 == rhs.kaz2 &&
        slowTimeDeskew == rhs.slowTimeDeskew);
}

}
}

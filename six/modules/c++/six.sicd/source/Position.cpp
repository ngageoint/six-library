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
#include "six/sicd/Position.h"
#include "six/sicd/SCPCOA.h"
#include <math/Utilities.h>

namespace six
{
namespace sicd
{
Position::Position() :
    arpPoly(Init::undefined<PolyXYZ>()),
    grpPoly(Init::undefined<PolyXYZ>()),
    txAPCPoly(Init::undefined<PolyXYZ>())
{
}

void Position::fillDerivedFields(const SCPCOA& scpcoa)
{
    if (!Init::isUndefined(scpcoa.arpPos) &&
        !Init::isUndefined(scpcoa.arpVel) &&
        !Init::isUndefined(scpcoa.scpTime) &&
        (Init::isUndefined(arpPoly)))
    {
        const Vector3 tempArpAcc = Init::isUndefined(scpcoa.arpAcc) ?
            Vector3(0.0) : scpcoa.arpAcc;

        std::vector<Vector3> coefs;
        coefs.resize(3);

        //constant
        coefs[0] = scpcoa.arpPos -
            scpcoa.arpVel * scpcoa.scpTime +
            (tempArpAcc / 2) * math::square(scpcoa.scpTime);

        //linear
        coefs[1] = scpcoa.arpVel -
            tempArpAcc * scpcoa.scpTime;

        //quadratic
        coefs[2] = tempArpAcc / 2;

        arpPoly = PolyXYZ(coefs);
    }
}

bool Position::validate(logging::Logger& log) const
{
    // 2.6
    if (arpPoly.order() < 2)
    {
        log.error("Position.arpPoly should have at least position"
                " and velocity terms.");
        return false;
    }
    return true;
}
}
}

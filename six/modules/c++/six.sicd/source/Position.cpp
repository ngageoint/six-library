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
    if (!Init::isUndefined<Vector3>(scpcoa.arpPos) &&
        !Init::isUndefined<Vector3>(scpcoa.arpVel) &&
        !Init::isUndefined<double>(scpcoa.scpTime) &&
        (Init::isUndefined<PolyXYZ>(arpPoly)))
    {
        Vector3 tempArpAcc = scpcoa.arpAcc;
        if (Init::isUndefined<Vector3>(scpcoa.arpAcc))
        {
            tempArpAcc = Vector3(0.0);
        }

        std::vector<Vector3> coefs;
        coefs.resize(3);

        //constant
        coefs[0] = scpcoa.arpPos -
            scpcoa.arpVel * scpcoa.scpTime +
            (tempArpAcc / 2) * std::pow(scpcoa.scpTime, 2);

        //linear
        coefs[1] = scpcoa.arpVel -
            tempArpAcc * scpcoa.scpTime;

        //quadratic
        coefs[2] = tempArpAcc / 2;

        arpPoly = PolyXYZ(coefs);
    }
}
}
}

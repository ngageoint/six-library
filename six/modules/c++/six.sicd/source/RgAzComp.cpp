/* =========================================================================
* This file is part of six.sicd-c++
* =========================================================================
*
* (C) Copyright 2004 - 2016, MDA Information Systems LLC
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

#include "six/sicd/GeoData.h"
#include "six/sicd/Grid.h"
#include "six/sicd/RgAzComp.h"
#include "six/sicd/SCPCOA.h"
#include "six/sicd/Timeline.h"
#include "six/sicd/Utilities.h"

namespace six
{
namespace sicd
{


void RgAzComp::fillDerivedFields(const GeoData& geoData,
        const Grid& grid,
        const SCPCOA& scpcoa,
        const Timeline& timeline)
{
    if (Init::isUndefined<double>(azSF))
    {
        azSF = std::sin(scpcoa.dopplerConeAngle *
            math::Constants::DEGREES_TO_RADIANS) / scpcoa.slantRange;
    }

    // Dervied: RgAzComp.KazPoly
    if (timeline.interPulsePeriod->sets.size() == 1 &&
        !Init::isUndefined<Poly1D>(
                timeline.interPulsePeriod->sets[0].interPulsePeriodPoly) &&
        !Init::isUndefined<double>(grid.row->kCenter) &&
        Init::isUndefined<Poly1D>(kazPoly))
    {
        double krgCoa = grid.row->kCenter;
        const Vector3& scp = geoData.scp.ecf;
        if (!Init::isUndefined<Poly2D>(grid.row->deltaKCOAPoly))
        {
            krgCoa += grid.row->deltaKCOAPoly.atY(scp[1])(scp[0]);
        }
        double stRateCoa = timeline.interPulsePeriod->sets[0].
                interPulsePeriodPoly.derivative()(scpcoa.scpTime);
        double deltaKazPerDeltaV = scpcoa.look(scp) * krgCoa *
                scpcoa.arpVel.norm() * std::sin(scpcoa.dopplerConeAngle *
                    math::Constants::DEGREES_TO_RADIANS)
                 / scpcoa.slantRange / stRateCoa;
        kazPoly = timeline.interPulsePeriod->sets[0].interPulsePeriodPoly
                * deltaKazPerDeltaV;
    }
}
}
}


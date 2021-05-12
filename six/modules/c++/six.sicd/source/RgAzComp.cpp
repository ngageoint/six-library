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
#include <math/Utilities.h>

namespace six
{
namespace sicd
{
void RgAzComp::fillDerivedFields(const GeoData& geoData,
        const Grid& grid,
        const SCPCOA& scpcoa,
        const Timeline& timeline)
{
    const Vector3& scp = geoData.scp.ecf;
    if (Init::isUndefined(azSF))
    {
        azSF = derivedAzSf(scpcoa, scp);
    }

    if (timeline.interPulsePeriod.get() &&
        timeline.interPulsePeriod->sets.size() == 1 &&
        !Init::isUndefined(
                timeline.interPulsePeriod->sets[0].interPulsePeriodPoly) &&
        !Init::isUndefined(grid.row->kCenter) &&
        Init::isUndefined(kazPoly))
    {
        kazPoly = derivedKazPoly(grid, scpcoa, timeline, scp);
    }

}

bool RgAzComp::validate(const GeoData& geoData,
        const Grid& grid,
        const SCPCOA& scpcoa,
        const Timeline& timeline,
        logging::Logger& log) const
{
    bool valid = true;
    std::ostringstream messageBuilder;
    const Vector3& scp = geoData.scp.ecf;

    //2.12.1.4
    const auto expectedAzSf = derivedAzSf(scpcoa, scp);
    if (std::abs(azSF - expectedAzSf) > 1e-6)
    {
        messageBuilder.str("");
        messageBuilder << "RGAZCOMP fields inconsistent." << std::endl
            << "RgAzComp.AzSF: " << azSF << std::endl
            << "Derived RgAzComp.AzSF: " << expectedAzSf;
        log.error(messageBuilder.str());
        valid = false;
    }

    if (timeline.interPulsePeriod->sets.size() == 1)
    {
        Poly1D derivedPoly = derivedKazPoly(grid, scpcoa, timeline, scp);
        Poly1D difference = kazPoly - derivedPoly;

        double norm = 0;
        for (size_t ii = 0; ii < difference.order(); ++ii)
        {
            norm += math::square(difference[ii]);
        }
        norm = std::sqrt(norm);

        if (std::sqrt(norm) > 1e-3)
        {
            messageBuilder.str("");
            messageBuilder << "RGAZCOMP fields inconsistent." << std::endl
                << "RgAzComp.KazPoly: " << kazPoly << std::endl
                << "Derived RgAzComp.KazPoly: " << derivedPoly;
            log.error(messageBuilder.str());
            valid = false;
        }
    }

    return valid;
}

double RgAzComp::derivedAzSf(const SCPCOA& scpcoa, const Vector3& scp) const
{
    return -scpcoa.look(scp) * std::sin(scpcoa.dopplerConeAngle *
        math::Constants::DEGREES_TO_RADIANS) / scpcoa.slantRange;
}

Poly1D RgAzComp::derivedKazPoly(const Grid& grid,
        const SCPCOA& scpcoa,
        const Timeline& timeline,
        const Vector3& scp) const
{
    const Poly1D& interPulsePeriodPoly = timeline.interPulsePeriod->sets[0].
        interPulsePeriodPoly;

    std::vector<double> krgValues((grid.row->deltaKCOAPoly.orderX() + 1) *
            (grid.row->deltaKCOAPoly.orderY() + 1), grid.row->kCenter);

    Poly2D krgCoa = grid.row->deltaKCOAPoly + Poly2D(
        (grid.row->deltaKCOAPoly.orderX()),
            (grid.row->deltaKCOAPoly.orderY()), krgValues);

    double stRateCoa = interPulsePeriodPoly.derivative()(scpcoa.scpTime);

    Poly2D deltaKazPerDeltaV = scpcoa.look(scp) * krgCoa *
        scpcoa.arpVel.norm() * std::sin(scpcoa.dopplerConeAngle *
            math::Constants::DEGREES_TO_RADIANS)
        / scpcoa.slantRange / stRateCoa;

    Poly1D derivedPoly(interPulsePeriodPoly.order());
    for (size_t row = 0; row < deltaKazPerDeltaV.orderX() + 1; ++row)
    {
        double currentValue = 0;
        for (size_t column = 0;
            column < deltaKazPerDeltaV.orderY() + 1; ++column)
        {
            currentValue += deltaKazPerDeltaV[row][column] *
                interPulsePeriodPoly[column];
        }
        derivedPoly[row] = currentValue;
    }
    return derivedPoly;
}
}
}


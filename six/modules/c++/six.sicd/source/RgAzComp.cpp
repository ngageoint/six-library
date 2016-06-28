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
    const Vector3& scp = geoData.scp.ecf;
    if (Init::isUndefined<double>(azSF))
    {
        azSF = derivedAzSf(scpcoa, scp);
    }

    if (timeline.interPulsePeriod->sets.size() == 1 &&
        !Init::isUndefined<Poly1D>(
                timeline.interPulsePeriod->sets[0].interPulsePeriodPoly) &&
        !Init::isUndefined<double>(grid.row->kCenter) &&
        Init::isUndefined<Poly1D>(kazPoly))
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
    double expectedAzSf = derivedAzSf(scpcoa, scp);
    if (std::abs(azSF - expectedAzSf) > 1e-6)
    {
        messageBuilder.str("");
        messageBuilder << "RGAZCOMP fields inconsistent." << std::endl
            << "RgAzComp.AzSF: " << azSF << std::endl
            << "Derived RgAzComp.AzSF: " << expectedAzSf;
        log.error(messageBuilder.str());
        valid = false;
    }

    // 2.12.1.5 omitted, polyval, polyder

    if (timeline.interPulsePeriod->sets.size() == 1)
    {
        Poly1D derivedPoly = derivedKazPoly(grid, scpcoa, timeline, scp);
        Poly1D difference = kazPoly - derivedPoly;

        double norm = 0;
        for (size_t ii = 0; ii < difference.order(); ++ii)
        {
            norm += std::pow(difference[ii], 2);
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
    double krgCoa = grid.row->kCenter;
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
    return timeline.interPulsePeriod->sets[0].interPulsePeriodPoly
        * deltaKazPerDeltaV;
}
}
}


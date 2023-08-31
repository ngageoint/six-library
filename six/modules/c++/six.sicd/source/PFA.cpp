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
#include "six/sicd/GeoData.h"
#include "six/sicd/Grid.h"
#include "six/sicd/PFA.h"
#include "six/sicd/Position.h"
#include "six/sicd/SCPCOA.h"
#include "six/sicd/Utilities.h"

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
        kaz1 == rhs.kaz1 &&
        kaz2 == rhs.kaz2 &&
        slowTimeDeskew == rhs.slowTimeDeskew);
}

void PFA::fillDerivedFields(const Position& position)
{
    if (!Init::isUndefined(position.arpPoly) &&
        !Init::isUndefined(polarAngleRefTime))
    {
        // This doesn't actually do anything currently.
        // Requires re-implemenation of polyfit, pfa_polar_coords, and bsxfun
        const Vector3 polRefPos = position.arpPoly(polarAngleRefTime);
    }
}

void PFA::fillDefaultFields(const GeoData& geoData,
        const Grid& grid, const SCPCOA& scpcoa)
{
    const Vector3& scp = geoData.scp.ecf;
    if (Init::isUndefined(imagePlaneNormal))
    {
        scene::WGS84EllipsoidModel model;
        switch (grid.imagePlane)
        {
        case ComplexImagePlaneType::SLANT:
        case ComplexImagePlaneType::NOT_SET:
            imagePlaneNormal = scpcoa.slantPlaneNormal(scp);
            break;
        case ComplexImagePlaneType::GROUND:
            imagePlaneNormal = model.getNormalVector(scp);
            break;
        case ComplexImagePlaneType::OTHER:
        default:
            // Nothing we can do
            break;
        }
    }

    if (Init::isUndefined(focusPlaneNormal))
    {
        scene::WGS84EllipsoidModel model;
        focusPlaneNormal = model.getNormalVector(scp);
    }

    if (!Init::isUndefined(scpcoa.scpTime) &&
        Init::isUndefined(polarAngleRefTime))
    {
        polarAngleRefTime = scpcoa.scpTime;
    }

    //Vector3 polRefPos = arpPos;
    //for when the other functions get implemented
}

bool PFA::validate(const SCPCOA& scpcoa, logging::Logger& log)
{
    bool valid = true;
    std::ostringstream messageBuilder;

    //2.12.2.3
    if (polarAngleRefTime - scpcoa.scpTime >
            std::numeric_limits<double>::epsilon())
    {
        messageBuilder.str("");
        messageBuilder << "Polar angle reference time and center of aperture "
            << "time for center are usuallly the same." << std::endl
            << "PFA.PolarAngRefTime: " << polarAngleRefTime << std::endl
            << "SCPCOA.SCPTime: " << scpcoa.scpTime;
        log.warn(messageBuilder.str());
        valid = false;
    }

    return valid;
}
}
}

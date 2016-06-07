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
#include "six/sicd/Position.h"
#include "six/sicd/SCPCOA.h"
#include "six/sicd/Utilities.h"

using namespace six;
using namespace six::sicd;

SCPCOA::SCPCOA() :
    scpTime(Init::undefined<double>()),
    arpPos(Init::undefined<Vector3>()),
    arpVel(Init::undefined<Vector3>()),
    arpAcc(Init::undefined<Vector3>()),
    sideOfTrack(Init::undefined<SideOfTrackType>()),
    slantRange(Init::undefined<double>()),
    groundRange(Init::undefined<double>()),
    dopplerConeAngle(Init::undefined<double>()),
    grazeAngle(Init::undefined<double>()),
    incidenceAngle(Init::undefined<double>()),
    twistAngle(Init::undefined<double>()),
    slopeAngle(Init::undefined<double>()),
    azimAngle(Init::undefined<double>()),
    layoverAngle(Init::undefined<double>())
{
}

bool SCPCOA::operator==(const SCPCOA& rhs) const
{
    return (scpTime == rhs.scpTime &&
        arpPos == rhs.arpPos &&
        arpVel == rhs.arpVel &&
        arpAcc == rhs.arpAcc &&
        sideOfTrack == rhs.sideOfTrack &&
        slantRange == rhs.slantRange &&
        groundRange == rhs.groundRange &&
        dopplerConeAngle == rhs.dopplerConeAngle &&
        grazeAngle == rhs.grazeAngle &&
        incidenceAngle == rhs.incidenceAngle &&
        twistAngle == rhs.twistAngle &&
        slopeAngle == rhs.slopeAngle &&
        azimAngle == rhs.azimAngle &&
        layoverAngle == rhs.layoverAngle);
}

void SCPCOA::fillDerivedFields(const GeoData& geoData,
        const Grid& grid,
        const Position& position)
{
    if (Init::isUndefined<double>(scpTime) &&
        !Init::isUndefined<Poly2D>(grid.timeCOAPoly))
    {
        //Tested manually
        scpTime = grid.timeCOAPoly[0][0];
    }

    if (!Init::isUndefined<PolyXYZ>(position.arpPoly) &&
        !Init::isUndefined<double>(scpTime))
    {
        std::vector<double> arpPosEcf(3);
        std::vector<double> arpVelEcf(3);
        std::vector<double> arpAccEcf(3);
        for (size_t ii = 0; ii < 3; ++ii)
        {
            Poly1D posPoly = sicd::Utilities::nPoly(position.arpPoly, ii);
            Poly1D velPoly = posPoly.derivative();
            Poly1D accPoly = velPoly.derivative();

            arpPosEcf[ii] = posPoly(scpTime);
            arpVelEcf[ii] = velPoly(scpTime);
            arpAccEcf[ii] = accPoly(scpTime);
        }
        if (Init::isUndefined<Vector3>(arpPos))
        {
            arpPos = Vector3(arpPosEcf);
        }

        if (Init::isUndefined<Vector3>(arpVel))
        {
            arpVel = Vector3(arpVelEcf);
        }

        if (Init::isUndefined<Vector3>(arpAcc))
        {
            arpAcc = Vector3(arpAccEcf);
        }
    }

    const Vector3& scp = geoData.scp.ecf;
    Vector3 uLOS = (scp - arpPos).unit();
    Vector3 left = cross(arpPos.unit(), arpVel.unit());
    int look = six::sicd::Utilities::sign(uLOS.dot(left));

    if (sideOfTrack == SideOfTrackType::NOT_SET)
    {
        if (look < 0)
        {
            sideOfTrack = SideOfTrackType::RIGHT;
        }
        else
        {
            sideOfTrack = SideOfTrackType::LEFT;
        }
    }

    if (Init::isUndefined<double>(slantRange))
    {
        slantRange = (scp - arpPos).norm();
    }
    if (Init::isUndefined<double>(groundRange))
    {
        groundRange = scp.norm() * std::acos(
                arpPos.dot(scp) / (scp.norm() * arpPos.norm()));
    }
    if (Init::isUndefined<double>(dopplerConeAngle))
    {
        dopplerConeAngle = std::acos(arpVel.unit().dot(uLOS)) *
                math::Constants::RADIANS_TO_DEGREES;
    }

    // Earth Tangent Plane (ETP) at the SCP is the plane tangent to the
    // surface of constant height above the WGS 84 ellipsoid (HAE) that
    // contains the SCP. The ETP is an approximation to the ground plane
    // at the SCP.
    Vector3 etp = Utilities::wgs84Norm(scp);
    if (Init::isUndefined<double>(grazeAngle))
    {
        // Angle between ground plane and line-of-sight vector
        grazeAngle = std::asin(etp.dot(uLOS * -1)) *
                math::Constants::RADIANS_TO_DEGREES;
    }
    if (Init::isUndefined<double>(incidenceAngle))
    {
        // Angle between ground plane normal and line of sight vector
        incidenceAngle = 90 - grazeAngle;
    }

    // Instantaneous slant plane unit normal at COA
    // (also called uSPZ in SICD spec)
    Vector3 slantPlaneNormal = cross(arpVel, uLOS).unit() * look;

    // Project range vector (from SCP toward ARP) onto ground plane
    Vector3 uGPX = (uLOS * -1) - etp * (etp.dot(uLOS * -1));
    uGPX = uGPX.unit();

    if (Init::isUndefined<double>(twistAngle))
    {
        // 1) Equations from SICD spec:
        Vector3 uGPY = cross(etp, uGPX);

        // Angle from +GPY axis to the +SPY axis in plane of incidence
        twistAngle = -1 * std::asin(uGPY.dot(slantPlaneNormal)) *
                math::Constants::RADIANS_TO_DEGREES;
    }

    if (Init::isUndefined<double>(slopeAngle))
    {
        // Angle between slant and ground planes
        slopeAngle = std::acos(etp.dot(slantPlaneNormal)) *
                math::Constants::RADIANS_TO_DEGREES;
    }

    std::vector<double> coordinates;
    coordinates.resize(3);
    coordinates[0] = 0;
    coordinates[1] = 0;
    coordinates[2] = 1;
    Vector3 zeroZeroOne(coordinates);

    // Project north onto ground plane
    Vector3 northGround = zeroZeroOne - (etp * etp.dot(zeroZeroOne));
    // Unit vector in ground plane in north direction
    Vector3 uNorth = northGround.unit();
    // Unit vector in ground plane in east direction
    Vector3 uEast = cross(uNorth, etp);

    if (Init::isUndefined<double>(azimAngle))
    {
        double azNorth = uGPX.dot(uNorth);
        double azEast = uGPX.dot(uEast);
        azimAngle = std::fmod(std::atan2(azEast, azNorth) *
            math::Constants::RADIANS_TO_DEGREES, 360);
    }

    if (Init::isUndefined<double>(layoverAngle))
    {
        Vector3 layoverGround = etp - 
                (slantPlaneNormal / (etp.dot(slantPlaneNormal)));
        double loNorth = layoverGround.dot(uNorth);
        double loEast = layoverGround.dot(uEast);
        layoverAngle = std::fmod(std::atan2(loEast, loNorth) *
            math::Constants::RADIANS_TO_DEGREES, 360);
    }
}

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
#include <math/Utilities.h>
#include "six/sicd/GeoData.h"
#include "six/sicd/Grid.h"
#include "six/sicd/Position.h"
#include "six/sicd/SCPCOA.h"
#include "six/sicd/Utilities.h"

using namespace six;
using namespace six::sicd;

const std::string SCPCOA::mScpcoaInconsistentString =
        "SCPCOA fields not consistent with other SICD metadata.";
const double SCPCOA::mScpcoaTol = 1e-2;

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

double SCPCOA::derivedSCPTime(const Grid& grid) const
{
    return grid.timeCOAPoly[0][0];
}

double SCPCOA::derivedSlantRange(const Vector3& scp) const
{
    return (scp - arpPos).norm();
}

double SCPCOA::derivedGroundRange(const Vector3& scp) const
{
    return scp.norm() * arpPos.angle(scp);
}

double SCPCOA::derivedDopplerConeAngle(const Vector3& scp) const
{
    return std::acos(arpVel.unit().dot(uLOS(scp))) *
            math::Constants::RADIANS_TO_DEGREES;
}

Vector3 SCPCOA::earthTangentPlane(const Vector3& scp) const
{
    return Utilities::wgs84Norm(scp);
}

double SCPCOA::derivedGrazeAngle(const Vector3& scp) const
{
    return std::asin(earthTangentPlane(scp).dot(uLOS(scp) * -1)) *
            math::Constants::RADIANS_TO_DEGREES;
}

double SCPCOA::derivedIncidenceAngle(const Vector3& scp) const
{
    return 90 - derivedGrazeAngle(scp);
}

double SCPCOA::derivedSlopeAngle(const Vector3& scp) const
{
    return std::acos(earthTangentPlane(scp).dot(slantPlaneNormal(scp))) *
            math::Constants::RADIANS_TO_DEGREES;
}

Vector3 SCPCOA::slantPlaneNormal(const Vector3& scp) const
{
    return cross(arpVel, uLOS(scp)).unit() * look(scp);
}

Vector3 SCPCOA::uGPX(const Vector3& scp) const
{
    return ((uLOS(scp) * -1) -
        earthTangentPlane(scp) *
        (earthTangentPlane(scp).dot(uLOS(scp) * -1))).unit();
}

Vector3 SCPCOA::uEast(const Vector3& scp) const
{
    return cross(uNorth(scp), earthTangentPlane(scp));
}

Vector3 SCPCOA::uNorth(const Vector3& scp) const
{
    std::vector<double> coordinates;
    coordinates.resize(3);
    coordinates[0] = 0;
    coordinates[1] = 0;
    coordinates[2] = 1;
    Vector3 base(coordinates);

    // Project north onto ground plane
    Vector3 northGround = base -
            (earthTangentPlane(scp) * earthTangentPlane(scp).dot(base));
    return northGround.unit();
}

double SCPCOA::derivedTwistAngle(const Vector3& scp) const
{
    // Angle from +GPY axis to the +SPY axis in plane of incidence
    Vector3 uGPY = cross(earthTangentPlane(scp), uGPX(scp));
    return -1 * std::asin(uGPY.dot(slantPlaneNormal(scp))) *
        math::Constants::RADIANS_TO_DEGREES;
}

double SCPCOA::derivedAzimAngle(const Vector3& scp) const
{
    double azNorth = uGPX(scp).dot(uNorth(scp));
    double azEast = uGPX(scp).dot(uEast(scp));
    double tempAngle = std::fmod(std::atan2(azEast, azNorth) *
            math::Constants::RADIANS_TO_DEGREES, 360);

    // squish range from (-360, 360) to [0, 360)
    return tempAngle >= 0 ? tempAngle : tempAngle + 360;
}

double SCPCOA::derivedLayoverAngle(const Vector3& scp) const
{
    Vector3 layoverGround = earthTangentPlane(scp) -
        (slantPlaneNormal(scp) / (earthTangentPlane(scp).dot(
                slantPlaneNormal(scp))));
    double loNorth = layoverGround.dot(uNorth(scp));
    double loEast = layoverGround.dot(uEast(scp));
    return std::fmod(std::atan2(loEast, loNorth) *
            math::Constants::RADIANS_TO_DEGREES, 360);
}

void SCPCOA::fillDerivedFields(const GeoData& geoData,
        const Grid& grid,
        const Position& position)
{
    if (Init::isUndefined<double>(scpTime) &&
        !Init::isUndefined<Poly2D>(grid.timeCOAPoly))
    {
        scpTime = derivedSCPTime(grid);
    }
    if (!Init::isUndefined<PolyXYZ>(position.arpPoly) &&
        !Init::isUndefined<double>(scpTime))
    {
        std::vector<Vector3> vectors = derivedArpVectors(position);
        if (Init::isUndefined<Vector3>(arpPos))
        {
            arpPos = vectors[0];
        }
        if (Init::isUndefined<Vector3>(arpVel))
        {
            arpVel = vectors[1];
        }
        if (Init::isUndefined<Vector3>(arpAcc))
        {
            arpAcc = vectors[2];
        }
    }

    const Vector3& scp = geoData.scp.ecf;
    if (sideOfTrack == SideOfTrackType::NOT_SET)
    {
        sideOfTrack = derivedSideOfTrack(geoData);
    }
    if (Init::isUndefined<double>(slantRange))
    {
        slantRange = derivedSlantRange(scp);
    }
    if (Init::isUndefined<double>(groundRange))
    {
        groundRange = derivedGroundRange(scp);
    }
    if (Init::isUndefined<double>(dopplerConeAngle))
    {
        dopplerConeAngle = derivedDopplerConeAngle(scp);
    }
    if (Init::isUndefined<double>(grazeAngle))
    {
        grazeAngle = derivedGrazeAngle(scp);
    }
    if (Init::isUndefined<double>(incidenceAngle))
    {
        incidenceAngle = derivedIncidenceAngle(scp);
    }
    if (Init::isUndefined<double>(twistAngle))
    {
        twistAngle = derivedTwistAngle(scp);
    }
    if (Init::isUndefined<double>(slopeAngle))
    {
        slopeAngle = derivedSlopeAngle(scp);
    }
    if (Init::isUndefined<double>(azimAngle))
    {
        azimAngle = derivedAzimAngle(scp);
    }
    if (Init::isUndefined<double>(layoverAngle))
    {
        layoverAngle = derivedLayoverAngle(scp);
    }
}

Vector3 SCPCOA::uLOS(const Vector3& scp) const
{
    return (scp - arpPos).unit();
}

Vector3 SCPCOA::left() const
{
    return cross(arpPos.unit(), arpVel.unit());
}

int SCPCOA::look(const Vector3& scp) const
{
    return math::sign(uLOS(scp).dot(left()));
}

std::vector<Vector3> SCPCOA::derivedArpVectors(const Position& position) const
{
    std::vector<Vector3> vectors(3);

    for (size_t ii = 0; ii < 3; ++ii)
    {
        Poly1D posPoly = sicd::Utilities::nPoly(position.arpPoly, ii);
        Poly1D velPoly = posPoly.derivative();
        Poly1D accPoly = velPoly.derivative();

        vectors[0][ii] = posPoly(scpTime);
        vectors[1][ii] = velPoly(scpTime);
        vectors[2][ii] = accPoly(scpTime);
    }
    return vectors;
}

SideOfTrackType SCPCOA::derivedSideOfTrack(const GeoData& geoData) const
{
    return look(geoData.scp.ecf) < 0 ? SideOfTrackType::RIGHT :
        SideOfTrackType::LEFT;
}

bool SCPCOA::validate(const GeoData& geoData,
        const Grid& grid,
        const Position& position,
        logging::Logger& log)
{
    std::ostringstream messageBuilder;
    bool valid = true;
    const Vector3& scp = geoData.scp.ecf;
    // 2.7.1
    // SCPTime has stricter tolerance because everything else depends on it
    if (std::abs(derivedSCPTime(grid) - scpTime) >
            std::numeric_limits<double>::epsilon())
    {
        messageBuilder.str("");
        messageBuilder << mScpcoaInconsistentString << "\n" <<
            "SCPCOA.scpTime: " << scpTime << std::endl <<
            "Derived scpTime: " << derivedSCPTime(grid) << std::endl;
        log.error(messageBuilder.str());
        valid = false;
    }

    std::vector<Vector3> arpVectors = derivedArpVectors(position);
    // 2.7.2
    valid = valid && compareFields(arpPos, arpVectors[0], "arpPos", log);

    // 2.7.3
    valid = valid &&  compareFields(arpVel, arpVectors[1], "arpVel", log);

    // 2.7.4 ARPAcc doesn't really matter and the instantaneous
    // ARPAcc can often vary from the ARPPoly polynomial derived one
    valid = valid && compareFields(arpAcc, arpVectors[2], "arpAcc", log);

    // 2.7.5 SideOfTrack
    if (sideOfTrack != derivedSideOfTrack(geoData))
    {
        messageBuilder.str("");
        messageBuilder << mScpcoaInconsistentString << "\n" <<
            "SCPCOA.SideOfTrack: " << sideOfTrack << std::endl <<
            "Derived SideOfTrack: " << derivedSideOfTrack(geoData)
            << std::endl;
        log.error(messageBuilder.str());
        valid = false;
    }

    // 2.7.6 - 2.7.14
    valid = valid && compareFields(slantRange, derivedSlantRange(scp),
            "slantRange", log);
    valid = valid && compareFields(groundRange, derivedGroundRange(scp),
            "groundRange", log);
    valid = valid && compareFields(dopplerConeAngle,
            derivedDopplerConeAngle(scp), "dopplerConeAngle", log);
    valid = valid && compareFields(grazeAngle, derivedGrazeAngle(scp),
            "grazeAngle", log);
    valid = valid && compareFields(incidenceAngle, derivedIncidenceAngle(scp),
            "incidenceAngle", log);
    valid = valid && compareFields(twistAngle, derivedTwistAngle(scp),
            "twistAngle", log);
    valid = valid && compareFields(slopeAngle, derivedSlopeAngle(scp),
            "slopeAngle", log);
    valid = valid && compareFields(azimAngle, derivedAzimAngle(scp),
            "azimAngle", log);
    valid = valid && compareFields(layoverAngle, derivedLayoverAngle(scp),
            "layoverAngle", log);
    return valid;
}

bool SCPCOA::compareFields(double given, double expected,
        const std::string& name, logging::Logger& log) const
{
    if (std::abs(given - expected) < mScpcoaTol)
    {
        return true;
    }

    std::ostringstream messageBuilder;
    messageBuilder << mScpcoaInconsistentString << "\n" <<
        "SCPCOA." << name << ": " << given << std::endl <<
        "Derived " << name << ": " << expected << std::endl;
    log.error(messageBuilder.str());
    return false;
}

bool SCPCOA::compareFields(Vector3 given, Vector3 expected,
        const std::string& name, logging::Logger& log) const
{
    if ((given - expected).norm() < mScpcoaTol)
    {
        return true;
    }

    std::ostringstream messageBuilder;
    messageBuilder << mScpcoaInconsistentString << "\n" <<
        "SCPCOA." << name << ": " << given << std::endl <<
        "Derived " << name << ": " << expected << std::endl;
    log.error(messageBuilder.str());
    return false;
}

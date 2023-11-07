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

const char SCPCOA::SCPCOA_INCONSISTENT_STR[] =
        "SCPCOA fields not consistent with other SICD metadata.";
const double SCPCOA::SCPCOA_TOL = 1e-2;

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

Vector3 SCPCOA::slantPlaneNormal(const Vector3& scp) const
{
    return -scene::SceneGeometry(arpVel, arpPos, scp).getSlantPlaneZ();
}

double SCPCOA::derivedTwistAngle(const scene::SceneGeometry& geometry) const
{
    // Angle from +GPY axis to the +SPY axis in plane of incidence
    const Vector3 uGPY = cross(geometry.getGroundPlaneNormal(),
            -geometry.getGroundRange().unit());
    return -1 * std::asin(uGPY.dot(geometry.getSlantPlaneZ())) *
        math::Constants::RADIANS_TO_DEGREES;
}

void SCPCOA::fillDerivedFields(const GeoData& geoData,
        const Grid& grid,
        const Position& position)
{
    if (Init::isUndefined(scpTime) &&
        !Init::isUndefined(grid.timeCOAPoly))
    {
        scpTime = derivedSCPTime(grid);
    }
    if (!Init::isUndefined(position.arpPoly) &&
        !Init::isUndefined(scpTime))
    {
        std::vector<Vector3> vectors = derivedArpVectors(position);
        if (Init::isUndefined(arpPos))
        {
            arpPos = vectors[0];
        }
        if (Init::isUndefined(arpVel))
        {
            arpVel = vectors[1];
        }
        if (Init::isUndefined(arpAcc))
        {
            arpAcc = vectors[2];
        }
    }

    const scene::SceneGeometry geometry(arpVel, arpPos, geoData.scp.ecf);

    const Vector3& scp = geoData.scp.ecf;
    if (sideOfTrack == SideOfTrackType::NOT_SET)
    {
        sideOfTrack = six::Enum::cast<six::SideOfTrackType>(geometry.getSideOfTrack());
    }
    if (Init::isUndefined(slantRange))
    {
        slantRange = derivedSlantRange(scp);
    }
    if (Init::isUndefined(groundRange))
    {
        groundRange = derivedGroundRange(scp);
    }
    if (Init::isUndefined(dopplerConeAngle))
    {
        dopplerConeAngle = geometry.getDopplerConeAngle();
    }
    if (Init::isUndefined(grazeAngle))
    {
        grazeAngle = std::abs(geometry.getETPGrazingAngle());
    }
    if (Init::isUndefined(incidenceAngle))
    {
        incidenceAngle = 90 - std::abs(geometry.getETPGrazingAngle());
    }
    if (Init::isUndefined(twistAngle))
    {
        twistAngle = derivedTwistAngle(geometry);
    }
    if (Init::isUndefined(slopeAngle))
    {
        slopeAngle = geometry.getETPSlopeAngle();
    }
    if (Init::isUndefined(azimAngle))
    {
        azimAngle = geometry.getAzimuthAngle();
    }
    if (Init::isUndefined(layoverAngle))
    {
        layoverAngle = geometry.getETPLayoverAngle();
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

bool SCPCOA::validate(const GeoData& geoData,
        const Grid& grid,
        const Position& position,
        logging::Logger& log)
{
    std::ostringstream messageBuilder;
    bool valid = true;
    const Vector3& scp = geoData.scp.ecf;
    const scene::SceneGeometry geometry(arpVel, arpPos, scp);
    // 2.7.1
    // SCPTime has stricter tolerance because everything else depends on it
    if (std::abs(derivedSCPTime(grid) - scpTime) >
            std::numeric_limits<double>::epsilon())
    {
        messageBuilder.str("");
        messageBuilder << SCPCOA_INCONSISTENT_STR << "\n" <<
            "SCPCOA.scpTime: " << scpTime << std::endl <<
            "Derived scpTime: " << derivedSCPTime(grid) << std::endl;
        log.error(messageBuilder);
        valid = false;
    }

    std::vector<Vector3> arpVectors = derivedArpVectors(position);
    // 2.7.2
    valid = compareFields(arpPos, arpVectors[0], "arpPos", log) && valid;

    // 2.7.3
    valid = compareFields(arpVel, arpVectors[1], "arpVel", log) && valid;

    // 2.7.4 ARPAcc doesn't really matter and the instantaneous
    // ARPAcc can often vary from the ARPPoly polynomial derived one
    valid = compareFields(arpAcc, arpVectors[2], "arpAcc", log) && valid;

    // 2.7.5 SideOfTrack
    if (sideOfTrack != geometry.getSideOfTrack())
    {
        messageBuilder.str("");
        messageBuilder << SCPCOA_INCONSISTENT_STR << "\n" <<
            "SCPCOA.SideOfTrack: " << sideOfTrack << std::endl <<
            "Derived SideOfTrack: " << geometry.getSideOfTrack()
            << std::endl;
        log.error(messageBuilder);
        valid = false;
    }

    // 2.7.6 - 2.7.14
    valid = compareFields(slantRange, derivedSlantRange(scp),
            "slantRange", log) && valid;
    valid = compareFields(groundRange, derivedGroundRange(scp),
            "groundRange", log) && valid;
    valid = compareFields(dopplerConeAngle, geometry.getDopplerConeAngle(),
            "dopplerConeAngle", log) && valid;
    valid = compareFields(grazeAngle, geometry.getETPGrazingAngle(),
            "grazeAngle", log) && valid;;
    valid = compareFields(incidenceAngle,
            90 - std::abs(geometry.getETPGrazingAngle()),
            "incidenceAngle", log) && valid;
    valid = compareFields(twistAngle, derivedTwistAngle(geometry),
            "twistAngle", log) && valid;
    valid = compareFields(slopeAngle, geometry.getETPSlopeAngle(),
            "slopeAngle", log) && valid;
    valid = compareFields(azimAngle, geometry.getAzimuthAngle(),
            "azimAngle", log) && valid;
    valid = compareFields(layoverAngle, geometry.getETPLayoverAngle(),
            "layoverAngle", log) && valid;
    return valid;
}

bool SCPCOA::compareFields(double given, double expected,
        const std::string& name, logging::Logger& log) const
{
    if (std::abs(given - expected) < SCPCOA_TOL)
    {
        return true;
    }

    std::ostringstream messageBuilder;
    messageBuilder << SCPCOA_INCONSISTENT_STR << "\n" <<
        "SCPCOA." << name << ": " << given << std::endl <<
        "Derived " << name << ": " << expected << std::endl;
    log.error(messageBuilder);
    return false;
}

bool SCPCOA::compareFields(Vector3 given, Vector3 expected,
        const std::string& name, logging::Logger& log) const
{
    if ((given - expected).norm() < SCPCOA_TOL)
    {
        return true;
    }

    std::ostringstream messageBuilder;
    messageBuilder << SCPCOA_INCONSISTENT_STR << "\n" <<
        "SCPCOA." << name << ": " << given << std::endl <<
        "Derived " << name << ": " << expected << std::endl;
    log.error(messageBuilder);
    return false;
}

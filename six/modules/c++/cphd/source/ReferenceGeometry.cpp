/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * cphd-c++ is free software; you can redistribute it and/or modify
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

#include <cphd/ReferenceGeometry.h>
#include <six/Init.h>

namespace cphd
{

SRP::SRP() :
    ecf(six::Init::undefined<Vector3>()),
    iac(six::Init::undefined<Vector3>())
{
}

ImagingType::ImagingType() :
    azimuthAngle(0),
    grazeAngle(0),
    twistAngle(0),
    slopeAngle(0),
    layoverAngle(0)
{
}

Monostatic::Monostatic() :
    sideOfTrack(six::Init::undefined<six::SideOfTrackType>()),
    slantRange(0),
    groundRange(0),
    dopplerConeAngle(0),
    incidenceAngle(0),
    arpPos(six::Init::undefined<Vector3>()),
    arpVel(six::Init::undefined<Vector3>())
{
}

Bistatic::Bistatic() :
    azimuthAngleRate(0),
    bistaticAngle(0),
    bistaticAngleRate(0)
{
}

Bistatic::PlatformParams::PlatformParams() :
    sideOfTrack(six::Init::undefined<six::SideOfTrackType>()),
    time(0),
    azimuthAngle(0),
    grazeAngle(0),
    incidenceAngle(0),
    dopplerConeAngle(0),
    groundRange(0),
    slantRange(0),
    pos(six::Init::undefined<Vector3>()),
    vel(six::Init::undefined<Vector3>())
{
}

ReferenceGeometry::ReferenceGeometry() :
    referenceTime(0),
    srpCODTime(0),
    srpDwellTime(0)
{
}

std::ostream& operator<< (std::ostream& os, const SRP& s)
{
    os << "SRP:: \n"
        << "  ECF              : " << s.ecf << "\n"
        << "  IAC              : " << s.iac << "\n";
    return os;
}

std::ostream& operator<< (std::ostream& os, const ImagingType& i)
{
    os << "  AzimuthAngle     : " << i.azimuthAngle << "\n"
        << "  GrazeAngle       : " << i.grazeAngle << "\n"
        << "  TwistAngle       : " << i.twistAngle << "\n"
        << "  SlopeAngle       : " << i.slopeAngle << "\n"
        << "  layoverAngle     : " << i.layoverAngle << "\n";
    return os;
}

std::ostream& operator<< (std::ostream& os, const Monostatic& m)
{
    os << "Monostatic:: \n"
        << (ImagingType)m << "\n"
        << "  SideOfTrack      : " << m.sideOfTrack.toString() << "\n"
        << "  SlantRange       : " << m.slantRange << "\n"
        << "  GroundRange      : " << m.groundRange << "\n"
        << "  DopplerConeAngle : " << m.dopplerConeAngle << "\n"
        << "  IncidenceAngle   : " << m.incidenceAngle << "\n"
        << "  ArpPos:: \n"
        << "    X              : " << m.arpPos[0] << "\n"
        << "    Y              : " << m.arpPos[1] << "\n"
        << "    Z              : " << m.arpPos[2] << "\n"
        << "  ArpVel:: \n"
        << "    X              : " << m.arpVel[0] << "\n"
        << "    Y              : " << m.arpVel[1] << "\n"
        << "    Z              : " << m.arpVel[2] << "\n";
    return os;
}

std::ostream& operator<< (std::ostream& os, const Bistatic::PlatformParams& p)
{
    os << "    SideOfTrack    : " << p.sideOfTrack.toString() << "\n"
        << "    Time:          : " << p.time << "\n"
        << "    AzimuthAngle   : " << p.azimuthAngle << "\n"
        << "    GrazeAngle     : " << p.grazeAngle << "\n"
        << "    IncidenceAngle : " << p.incidenceAngle << "\n"
        << "    DopplerConeAngle : " << p.dopplerConeAngle << "\n"
        << "    GroundRange    : " << p.groundRange << "\n"
        << "    SlantRange     : " << p.slantRange << "\n"
        << "    Pos:: \n"
        << "      X            : " << p.pos[0] << "\n"
        << "      Y            : " << p.pos[1] << "\n"
        << "      Z            : " << p.pos[2] << "\n"
        << "    Vel:: \n"
        << "      X            : " << p.vel[0] << "\n"
        << "      Y            : " << p.vel[1] << "\n"
        << "      Z            : " << p.vel[2] << "\n";

    return os;
}

std::ostream& operator<< (std::ostream& os, const Bistatic& b)
{
    os << "Bistatic:: \n"
        << (ImagingType)b << "\n"
        << "  AzimuthAngleRate : " << b.azimuthAngleRate << "\n"
        << "  BistaticAngle    : " << b.bistaticAngle << "\n"
        << "  BistaticAngleRate : " << b.bistaticAngleRate << "\n"
        << "  TxPlatform:: \n"
        << b.txPlatform << "\n"
        << b.rcvPlatform << "\n";
    return os;
}

std::ostream& operator<< (std::ostream& os, const ReferenceGeometry& r)
{
    os << "Reference Geometry:: \n"
        << "  ReferenceTime    : " << r.referenceTime << "\n"
        << "  SRPCODTime       : " << r.srpCODTime << "\n"
        << "  SRPDwellTime     : " << r.srpDwellTime << "\n"
        << r.srp;

    if (r.monostatic.get())
    {
        os << *r.monostatic << "\n";
    }
    else if (r.bistatic.get())
    {
        os << *r.bistatic << "\n";
    }
    return os;
}
}

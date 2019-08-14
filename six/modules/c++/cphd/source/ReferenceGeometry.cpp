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
{}

ImagingType::ImagingType() :
    azimuthAngle(six::Init::undefined<double>()),
    grazeAngle(six::Init::undefined<double>()),
    twistAngle(six::Init::undefined<double>()),
    slopeAngle(six::Init::undefined<double>()),
    layoverAngle(six::Init::undefined<double>())
{}

Monostatic::Monostatic() :
    sideOfTrack(six::Init::undefined<six::SideOfTrackType>()),
    slantRange(six::Init::undefined<double>()),
    groundRange(six::Init::undefined<double>()),
    dopplerConeAngle(six::Init::undefined<double>()),
    incidenceAngle(six::Init::undefined<double>()),
    arpPos(six::Init::undefined<Vector3>()),
    arpVel(six::Init::undefined<Vector3>())

{}

Bistatic::Bistatic() :
    azimuthAngleRate(six::Init::undefined<double>()),
    bistaticAngle(six::Init::undefined<double>()),
    bistaticAngleRate(six::Init::undefined<double>())
{}

Bistatic::PlatformParams::PlatformParams() :
    sideOfTrack(six::Init::undefined<six::SideOfTrackType>()),
    time(six::Init::undefined<double>()),
    azimuthAngle(six::Init::undefined<double>()),
    grazeAngle(six::Init::undefined<double>()),
    incidenceAngle(six::Init::undefined<double>()),
    dopplerConeAngle(six::Init::undefined<double>()),
    groundRange(six::Init::undefined<double>()),
    slantRange(six::Init::undefined<double>()),
    pos(six::Init::undefined<Vector3>()),
    vel(six::Init::undefined<Vector3>())

{}

ReferenceGeometry::ReferenceGeometry() :
    referenceTime(six::Init::undefined<double>()),
    srpCODTime(six::Init::undefined<double>()),
    srpDwellTime(six::Init::undefined<double>())
{}

}

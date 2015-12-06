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
#include "six/sicd/SCPCOA.h"

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


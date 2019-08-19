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

#include <cphd/ErrorParameters.h>
#include <six/Init.h>

namespace cphd
{
ErrorParameters::Monostatic::RadarSensor::RadarSensor() :
    rangeBias(six::Init::undefined<double>()),
    clockFreqSF(six::Init::undefined<double>()),
    collectionStartTime(six::Init::undefined<double>())
{
}

ErrorParameters::Monostatic::TropoError::TropoError() :
    tropoRangeVertical(six::Init::undefined<double>()),
    tropoRangeSlant(six::Init::undefined<double>())
{
}

ErrorParameters::Monostatic::IonoError::IonoError() :
    ionoRangeVertical(six::Init::undefined<double>()),
    ionoRangeRateVertical(six::Init::undefined<double>()),
    ionoRgRgRateCC(six::Init::undefined<double>())
{
}

ErrorParameters::Bistatic::RadarSensor::RadarSensor() :
    clockFreqSF(six::Init::undefined<double>()),
    collectionStartTime(six::Init::undefined<double>())
{
}

PosVelErr::PosVelErr() :
    frame(six::Init::undefined<scene::FrameType>()),
    p1(six::Init::undefined<double>()),
    p2(six::Init::undefined<double>()),
    p3(six::Init::undefined<double>()),
    v1(six::Init::undefined<double>()),
    v2(six::Init::undefined<double>()),
    v3(six::Init::undefined<double>())
{
}

PosVelErr::CorrCoefs::CorrCoefs() :
    p1p2(six::Init::undefined<double>()),
    p1p3(six::Init::undefined<double>()),
    p1v1(six::Init::undefined<double>()),
    p1v2(six::Init::undefined<double>()),
    p1v3(six::Init::undefined<double>()),
    p2p3(six::Init::undefined<double>()),
    p2v1(six::Init::undefined<double>()),
    p2v2(six::Init::undefined<double>()),
    p2v3(six::Init::undefined<double>()),
    p3v1(six::Init::undefined<double>()),
    p3v2(six::Init::undefined<double>()),
    p3v3(six::Init::undefined<double>()),
    v1v2(six::Init::undefined<double>()),
    v1v3(six::Init::undefined<double>()),
    v2v3(six::Init::undefined<double>())
{
}

Decorr::Decorr() :
    corrCoefZero(six::Init::undefined<double>()),
    decorrRate(six::Init::undefined<double>())
{
}

}

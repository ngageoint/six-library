/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2013, General Dynamics - Advanced Information Systems
 *
 * six-c++ is free software; you can redistribute it and/or modify
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
#include <six/ErrorStatistics.h>

namespace six
{
RadarSensor::RadarSensor() :
    rangeBias(Init::undefined<double>()),
    clockFreqSF(Init::undefined<double>()),
    transmitFreqSF(Init::undefined<double>()),
    rangeBiasDecorr(Init::undefined<DecorrType>())
{
}

TropoError::TropoError() :
    tropoRangeVertical(Init::undefined<double>()),
    tropoRangeSlant(Init::undefined<double>()),
    tropoRangeDecorr(Init::undefined<DecorrType>())
{
}

IonoError::IonoError() :
    ionoRangeVertical(Init::undefined<double>()),
    ionoRangeRateVertical(Init::undefined<double>()),
    ionoRgRgRateCC(Init::undefined<double>()),
    ionoRangeVertDecorr(Init::undefined<DecorrType>())
{
}
}

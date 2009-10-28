/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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
#include "six/ErrorStatistics.h"

using namespace six;

PosVelError* PosVelError::clone()
{
    PosVelError* e = new PosVelError();
    e->p1 = p1;
    e->p2 = p2;
    e->p3 = p3;
    e->v1 = v1;
    e->v2 = v2;
    e->v3 = v3;
    e->frame = frame;

    if (corrCoefs)
    {
        e->corrCoefs = corrCoefs->clone();
    }
    return e;
}

RadarSensor::RadarSensor()
{
    rangeBias = Init::undefined<double>();
    clockFreqSF = Init::undefined<double>();
    transmitFreqSF = Init::undefined<double>();
    rangeBiasDecorr = Init::undefined<DecorrType>();
}

RadarSensor* RadarSensor::clone() const
{
    return new RadarSensor(*this);
}

TropoError::TropoError()
{
    tropoRangeVertical = Init::undefined<double>();
    tropoRangeSlant = Init::undefined<double>();
    tropoRangeDecorr = Init::undefined<DecorrType>();
}

TropoError* TropoError::clone() const
{
    return new TropoError(*this);
}

IonoError::IonoError()
{
    ionoRangeVertical = Init::undefined<double>();
    ionoRangeRateVertical = Init::undefined<double>();
    ionoRgRgRateCC = Init::undefined<double>();
    ionoRangeVertDecorr = Init::undefined<DecorrType>();
}

IonoError* IonoError::clone() const
{
    return new IonoError(*this);
}

Components::~Components()
{
    if (posVelError)
        delete posVelError;
    if (radarSensor)
        delete radarSensor;
    if (tropoError)
        delete tropoError;
    if (ionoError)
        delete ionoError;
}

Components* Components::clone() const
{
    Components* c = new Components();
    if (posVelError)
        c->posVelError = posVelError->clone();
    if (radarSensor)
        c->radarSensor = radarSensor->clone();
    if (tropoError)
        c->tropoError = tropoError->clone();
    if (ionoError)
        c->ionoError = ionoError->clone();
    return c;
}

CompositeSCP* CompositeSCP::clone() const
{
    CompositeSCP* c = new CompositeSCP();
    c->xErr = xErr;
    c->yErr = yErr;
    c->xyErr = xyErr;
    return c;
}

ErrorStatistics::~ErrorStatistics()
{
    if (components)
        delete components;
    if (compositeSCP)
        delete compositeSCP;
}

ErrorStatistics* ErrorStatistics::clone() const
{
    ErrorStatistics* e = new ErrorStatistics();
    e->scpType = scpType;
    e->additionalParameters = additionalParameters;
    if (components)
    {
        e->components = components->clone();
    }
    if (compositeSCP)
    {
        e->compositeSCP = compositeSCP->clone();
    }
    return e;
}

void ErrorStatistics::initialize(SCPType type)
{
    scpType = scpType;
}

template<> std::string str::toString(const six::PosVelError::FrameType& value)
{
    switch (value)
    {
    case six::PosVelError::FRAME_ECF:
        return "ECF";
    case six::PosVelError::FRAME_RIC_ECF:
        return "RIC_ECF";
    case six::PosVelError::FRAME_RIC_ECI:
        return "RIC_ECI";
    default:
        throw except::Exception(Ctxt("Unsupported frame type"));
    }
}

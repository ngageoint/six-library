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
#include <mem/ScopedCopyablePtr.h>
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

std::ostream& operator<< (std::ostream& os, const Decorr& d)
{
    os << "      CorrCoefZero   : " << d.corrCoefZero << "\n"
        << "      DecorrRate    : " << d.decorrRate << "\n";
    return os;
}

std::ostream& operator<< (std::ostream& os, const PosVelErr::CorrCoefs& c)
{
    os << "      CorrCoefs:: \n"
        << "      P1P2         : " << c.p1p2 << "\n"
        << "      P1P3         : " << c.p1p3 << "\n"
        << "      P1V1         : " << c.p1v1 << "\n"
        << "      P1V2         : " << c.p1v2 << "\n"
        << "      P1V3         : " << c.p1v3 << "\n"
        << "      P2P3         : " << c.p2p3 << "\n"
        << "      P2V1         : " << c.p2v1 << "\n"
        << "      P2V2         : " << c.p2v2 << "\n"
        << "      P2V3         : " << c.p2v3 << "\n"
        << "      P3V1         : " << c.p3v1 << "\n"
        << "      P3V2         : " << c.p3v2 << "\n"
        << "      P3V3         : " << c.p3v3 << "\n"
        << "      V1V2         : " << c.v1v2 << "\n"
        << "      V1V3         : " << c.v1v3 << "\n"
        << "      V2V3         : " << c.v2v3 << "\n";
    return os;
}

std::ostream& operator<< (std::ostream& os, const PosVelErr& p)
{
    os << "    PosVelErr:: \n"
        << "      Frame          : " << p.frame.toString() << "\n"
        << "      P1             : " << p.p1 << "\n"
        << "      P2             : " << p.p2 << "\n"
        << "      P3             : " << p.p3 << "\n"
        << "      V1             : " << p.v1 << "\n"
        << "      V2             : " << p.v2 << "\n"
        << "      V3             : " << p.v3 << "\n";
    if(p.corrCoefs.get())
    {
        os << *(p.corrCoefs) << "\n";
    }
    os << "      PostionDecorr:: \n";
    if(p.positionDecorr.get())
    {
        os << *(p.positionDecorr);
    }
    return os;
}

std::ostream& operator<< (std::ostream& os, const ErrorParameters& e)
{
    os << "ErrorParameters:: \n";
    if (e.monostatic.get())
    {
        os << "  Monostatic:: \n"
            << e.monostatic->posVelErr << "\n"
            << "    RadarSensor:: \n"
            << "    RangeBias        : " << e.monostatic->radarSensor.rangeBias << "\n"
            << "    ClockFreqSF      : " << e.monostatic->radarSensor.clockFreqSF << "\n"
            << "    CollectionStartTime : " << e.monostatic->radarSensor.collectionStartTime << "\n"
            << "    RangeBiasDecorr:: \n";
        if (e.monostatic->radarSensor.rangeBiasDecorr.get())
        {
            os << *(e.monostatic->radarSensor.rangeBiasDecorr) << "\n";
        }
        if (e.monostatic->tropoError.get())
        {
            os << "    TropoError:: \n"
                << "    TropoRangeVertical : " << e.monostatic->tropoError->tropoRangeVertical << "\n"
                << "    TropoRangeSlant  : " << e.monostatic->tropoError->tropoRangeSlant << "\n"
                << "    TropoRangeDecorr:: \n";
            if (e.monostatic->tropoError->tropoRangeDecorr.get())
            {
                os << *(e.monostatic->tropoError->tropoRangeDecorr) << "\n";
            }
        }
        if (e.monostatic->ionoError.get())
        {
            os << "    IonoError:: \n"
                << "    IonoRangeVertical : " << e.monostatic->ionoError->ionoRangeVertical << "\n"
                << "    IonoRangeRateVertical : " << e.monostatic->ionoError->ionoRangeRateVertical << "\n"
                << "    IonoRgRgRateCC   : " << e.monostatic->ionoError->ionoRgRgRateCC << "\n"
                << "    IonoRangeVertDecorr:: \n";
            if (e.monostatic->ionoError->ionoRangeVertDecorr.get())
            {
                os << *(e.monostatic->ionoError->ionoRangeVertDecorr) << "\n";
            }
        }
        for (size_t ii = 0; ii < e.monostatic->parameter.size(); ++ii)
        {
            os << "    Parameter Name   : " << e.monostatic->parameter[ii].getName() << "\n"
                << "    Parameter Value  : " << e.monostatic->parameter[ii].str() << "\n";
        }
    }
    else if (e.bistatic.get())
    {
        os << "  Bistatic:: \n"
            << "    TxPlatform:: \n"
            << e.bistatic->txPlatform.posVelErr << "\n"
            << "    RadarSensor:: \n"
            << "    ClockFreqSF      : " << e.bistatic->txPlatform.radarSensor.clockFreqSF << "\n"
            << "    CollectionStartTime : " << e.bistatic->txPlatform.radarSensor.collectionStartTime << "\n"
            << "\n"
            << "    RcvPlatform:: \n"
            << e.bistatic->rcvPlatform.posVelErr << "\n"
            << "    RadarSensor:: \n"
            << "    ClockFreqSF      : " << e.bistatic->rcvPlatform.radarSensor.clockFreqSF << "\n"
            << "    CollectionStartTime : " << e.bistatic->rcvPlatform.radarSensor.collectionStartTime << "\n";
        for (size_t ii = 0; ii < e.bistatic->parameter.size(); ++ii)
        {
            os << "    Parameter Name   : " << e.bistatic->parameter[ii].getName() << "\n"
                << "    Parameter Value  : " << e.bistatic->parameter[ii].str() << "\n";
        }
    }
    else
    {
            throw except::Exception(Ctxt(
                    "One of either monostatic or bistatic is required"));
    }
    return os;
}
}

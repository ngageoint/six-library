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

#include <mem/ScopedCopyablePtr.h>
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

ErrorParameters::Bistatic::RadarSensor::RadarSensor() :
    clockFreqSF(six::Init::undefined<double>()),
    collectionStartTime(six::Init::undefined<double>())
{
}

static inline std::ostream& operator<< (std::ostream& os, const std::optional<double>& value)
{
    os << value.value();
    return os;
}

std::ostream& operator<< (std::ostream& os, const six::PosVelError& p)
{
    os << "    PosVelError:: \n"
        << "    Frame          : " << p.frame << "\n"
        << "    P1             : " << p.p1 << "\n"
        << "    P2             : " << p.p2 << "\n"
        << "    P3             : " << p.p3 << "\n"
        << "    V1             : " << p.v1 << "\n"
        << "    V2             : " << p.v2 << "\n"
        << "    V3             : " << p.v3 << "\n";
        if (p.corrCoefs.get())
        {
            os << "    CorrCoefs:: \n"
                << "    p1p2          : " << p.corrCoefs->p1p2 << "\n"
                << "    p1p3          : " << p.corrCoefs->p1p3 << "\n"
                << "    p1v1          : " << p.corrCoefs->p1v1 << "\n"
                << "    p1v2          : " << p.corrCoefs->p1v2 << "\n"
                << "    p1v3          : " << p.corrCoefs->p1v3 << "\n"
                << "    p2p3          : " << p.corrCoefs->p2p3 << "\n"
                << "    p2v1          : " << p.corrCoefs->p2v1 << "\n"
                << "    p2v2          : " << p.corrCoefs->p2v2 << "\n"
                << "    p2v3          : " << p.corrCoefs->p2v3 << "\n"
                << "    p3v1          : " << p.corrCoefs->p3v1 << "\n"
                << "    p3v2          : " << p.corrCoefs->p3v2 << "\n"
                << "    p3v3          : " << p.corrCoefs->p3v3 << "\n"
                << "    v1v2          : " << p.corrCoefs->v1v2 << "\n"
                << "    v1v3          : " << p.corrCoefs->v1v3 << "\n"
                << "    v2v3          : " << p.corrCoefs->v2v3 << "\n";
        }
        os << "    Decorr:: \n"
            << "    CorrCoefZero   : " << p.PositionDecorr().get().corrCoefZero << "\n"
            << "    DecorrRate     : " << p.PositionDecorr().get().decorrRate << "\n";
    return os;
}

std::ostream& operator<< (std::ostream& os, const ErrorParameters& e)
{
    os << "Error Parameters:: \n";
    if (e.monostatic.get())
    {
        os << "  Monostatic:: \n"
            << e.monostatic->posVelErr << "\n"
            << "    RadarSensor:: \n"
            << "    RangeBias        : " << e.monostatic->radarSensor.rangeBias << "\n";
        if (!six::Init::isUndefined(e.monostatic->radarSensor.clockFreqSF))
        {
            os << "    ClockFreqSF      : " << e.monostatic->radarSensor.clockFreqSF << "\n";
        }
        if (!six::Init::isUndefined(e.monostatic->radarSensor.collectionStartTime))
        {
            os << "    CollectionStartTime : " << e.monostatic->radarSensor.collectionStartTime << "\n";
        }
        if (e.monostatic->radarSensor.rangeBiasDecorr.get())
        {
            os << "    RangeBiasDecorr:: \n"
                << "    CorrCoefZero   : " << e.monostatic->radarSensor.rangeBiasDecorr->corrCoefZero << "\n"
                << "    DecorrRate     : " << e.monostatic->radarSensor.rangeBiasDecorr->decorrRate << "\n";
        }
        if (e.monostatic->tropoError.get())
        {
            os << "    TropoError:: \n";
            if (e.monostatic->tropoError->TropoRangeVertical().has())
            {
                os << "    TropoRangeVertical : " << e.monostatic->tropoError->TropoRangeVertical().get() << "\n";
            }
            if (e.monostatic->tropoError->TropoRangeSlant().has())
            {
                os << "    TropoRangeSlant  : " << e.monostatic->tropoError->TropoRangeSlant().get() << "\n";
            }
            if (e.monostatic->tropoError->TropoRangeDecorr().has())
            {
            os << "    TropoRangeDecorr:: \n"
                << "      CorrCoefZero  : " << e.monostatic->tropoError->TropoRangeDecorr().get().corrCoefZero << "\n"
                << "      DecorrRate  : " << e.monostatic->tropoError->TropoRangeDecorr().get().decorrRate << "\n";
            }
        }
        if (e.monostatic->ionoError.get())
        {
            os << "    IonoError:: \n";
            if (e.monostatic->ionoError->IonoRangeVertical().has())
            {
                os << "    IonoRangeVertical : " << e.monostatic->ionoError->IonoRangeVertical().get() << "\n";
            }
            if (e.monostatic->ionoError->IonoRangeRateVertical().has())
            {
                os << "    IonoRangeRateVertical  : " << e.monostatic->ionoError->IonoRangeRateVertical().get() << "\n";
            }
            os << "    IonoRgRgRateCC    : " << e.monostatic->ionoError->IonoRgRgRateCC().get() << "\n";
            if (e.monostatic->ionoError->IonoRangeVertDecorr().has())
            {
            os << "    IonoRangeDecorr:: \n"
                << "      CorrCoefZero  : " << e.monostatic->ionoError->IonoRangeVertDecorr().get().corrCoefZero << "\n"
                << "      DecorrRate  : " << e.monostatic->ionoError->IonoRangeVertDecorr().get().decorrRate << "\n";
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

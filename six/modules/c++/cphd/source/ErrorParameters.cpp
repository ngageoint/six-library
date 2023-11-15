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

static std::ostream& unchecked(std::ostream& os, const six::DecorrType& decorr_)
{
    const auto decorr = six::value(decorr_);
    os << "    CorrCoefZero   : " << decorr.corrCoefZero << "\n"
        << "    DecorrRate     : " << decorr.decorrRate << "\n";
    return os;
}
std::ostream& unchecked(std::ostream& os, const std::optional<six::DecorrType>& decorr)
{
    return unchecked(os, *decorr);
}

static std::ostream& checked(std::ostream& os, const std::string& s, const six::DecorrType& decorr)
{
    if (six::Init::isDefined(decorr))
    {
        os << s; 
        return unchecked(os, decorr);
    }
    return os;
}
std::ostream& checked(std::ostream& os, const std::string& s, const std::optional<six::DecorrType>& decorr)
{
    return decorr.has_value() ? checked(os, s, *decorr) : os;
}


static std::ostream& checked(std::ostream& os, const std::string& s, const double& v)
{
    if (six::Init::isDefined(v))
    {
        os << s << v << "\n";
    }
    return os;
}
std::ostream& checked(std::ostream& os, const std::string& s, const std::optional<double>& v)
{
    return v.has_value() ? checked(os, s, *v) : os;
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
        os << "    Decorr:: \n";
        return unchecked(os, p.positionDecorr);
}

static std::ostream& operator<< (std::ostream& os, const ErrorParameters::Bistatic::RadarSensor& v)
{
    os << "    RadarSensor:: \n"
        << v.delayBias 
        << "    ClockFreqSF      : " << v.clockFreqSF << "\n"
        << "    CollectionStartTime : " <<v.collectionStartTime << "\n";
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
            checked(os, "    TropoRangeVertical : ", e.monostatic->tropoError->tropoRangeVertical);
            checked(os, "    TropoRangeSlant  : ", e.monostatic->tropoError->tropoRangeSlant);
            checked(os, "    TropoRangeDecorr:: \n", e.monostatic->tropoError->tropoRangeDecorr);
        }
        if (e.monostatic->ionoError.get())
        {
            os << "    IonoError:: \n";
            checked(os, "    IonoRangeVertical : ", e.monostatic->ionoError->ionoRangeVertical);
            checked(os, "    IonoRangeRateVertical  : ", e.monostatic->ionoError->ionoRangeRateVertical);
            os << "    IonoRgRgRateCC    : " << six::value(e.monostatic->ionoError->ionoRgRgRateCC) << "\n";
            checked(os, "    IonoRangeDecorr:: \n", e.monostatic->ionoError->ionoRangeVertDecorr);
        }
        for (const auto& parameter : e.monostatic->parameter)
        {
            out(os, parameter);
        }
    }
    else if (e.bistatic.get())
    {
        os << "  Bistatic:: \n"
            << "    TxPlatform:: \n"
            << e.bistatic->txPlatform.posVelErr << "\n"
            << e.bistatic->txPlatform.radarSensor
            << "\n"
            << "    RcvPlatform:: \n"
            << e.bistatic->rcvPlatform.posVelErr << "\n"
            << e.bistatic->rcvPlatform.radarSensor;
        for (const auto& parameter : e.bistatic->parameter)
        {
            out(os, parameter);
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

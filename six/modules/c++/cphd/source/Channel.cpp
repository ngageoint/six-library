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
#include <cphd/Channel.h>

#include <std/string>

#include <str/Encoding.h>

#include <six/Init.h>

namespace cphd
{

TOAExtended::TOAExtended() :
    toaExtSaved(six::Init::undefined<double>())
{
}

TOAExtended::LFMEclipse::LFMEclipse() :
    fxEarlyLow(six::Init::undefined<double>()),
    fxEarlyHigh(six::Init::undefined<double>()),
    fxLateLow(six::Init::undefined<double>()),
    fxLateHigh(six::Init::undefined<double>())
{
}

DwellTimes::DwellTimes()
{
}

TgtRefLevel::TgtRefLevel() :
    ptRef(six::Init::undefined<double>())
{
}

Point::Point() :
    fx(six::Init::undefined<double>()),
    pn(six::Init::undefined<double>())
{
}

NoiseLevel::NoiseLevel() :
    pnRef(six::Init::undefined<double>()),
    bnRef(six::Init::undefined<double>())
{
}

ChannelParameter::ChannelParameter() :
    refVectorIndex(six::Init::undefined<size_t>()),
    fxFixed(six::Init::undefined<six::BooleanType>()),
    toaFixed(six::Init::undefined<six::BooleanType>()),
    srpFixed(six::Init::undefined<six::BooleanType>()),
    signalNormal(six::Init::undefined<six::BooleanType>()),
    fxC(six::Init::undefined<double>()),
    fxBW(six::Init::undefined<double>()),
    fxBWNoise(six::Init::undefined<double>()),
    toaSaved(six::Init::undefined<double>())
{
}

ChannelParameter::Antenna::Antenna()
{
}

Channel::Channel() :
    fxFixedCphd(six::Init::undefined<six::BooleanType>()),
    toaFixedCphd(six::Init::undefined<six::BooleanType>()),
    srpFixedCphd(six::Init::undefined<six::BooleanType>())
{
}

std::ostream& operator<< (std::ostream& os, const PolRef& v)
{
    os << v.ampH << v.ampV << v.phaseV << "\n";
    return os;
}

std::ostream& operator<< (std::ostream& os, const Polarization& p)
{
    os << "      TxPol        : " << p.txPol << "\n"
        << "      RcvPol       : " << p.rcvPol << "\n"
        << p.txPolRef << p.rcvPolRef;
    return os;
}

std::ostream& operator<< (std::ostream& os, const TOAExtended& t)
{
    os << "      TOAExtended:: \n"
        << "      TOAExtSaved  : " << t.toaExtSaved << "\n"
        << "      LFMEclipse:: \n"
        << "        FxEarlyLow : " << t.lfmEclipse->fxEarlyLow << "\n"
        << "        FxEarlyHigh : " << t.lfmEclipse->fxEarlyHigh << "\n"
        << "        FxLateLow : " << t.lfmEclipse->fxLateLow << "\n"
        << "        FxLateHigh : " << t.lfmEclipse->fxLateHigh << "\n";
    return os;
}

std::ostream& operator<< (std::ostream& os, const DwellTimes& d)
{
    os << "      DwellTimes:: \n"
        << "      CODId        : " << d.codId << "\n"
        << "      DwellId        : " << d.dwellId << "\n"
        << d.dtaId
        << d.useDTA;
    return os;
}

std::ostream& operator<< (std::ostream& os, const TgtRefLevel& t)
{
    os << "      TgtRefLevel:: \n"
        << "      PtRef        : " << t.ptRef << "\n";
        return os;
}

std::ostream& operator<< (std::ostream& os, const Point& p)
{
    os << "      Point:: \n"
        << "      Fx        : " << p.fx << "\n"
        << "      Pn        : " << p.pn << "\n";
        return os;
}

std::ostream& operator<< (std::ostream& os, const FxNoiseProfile& f)
{
    os << "        FxNoiseProfile:: \n";
    for (const auto& point : f.point)
    {
        os << "        Point        : " << point << "\n";
    }
    return os;
}

std::ostream& operator<< (std::ostream& os, const NoiseLevel& n)
{
    os << "      NoiseLevel:: \n"
        << "      PnRef        : " << n.pnRef << "\n"
        << "      BnRef        : " << n.bnRef << "\n";
    if(n.fxNoiseProfile.get())
    {
        os << *(n.fxNoiseProfile) << "\n";
    }
    return os;
}

std::ostream& operator<< (std::ostream& os, const ChannelParameter& c)
{
    os << "    ChannelParameter:: \n"
        << "      Identifier   : " << c.identifier << "\n"
        << "      RefVectorIndex : " << c.refVectorIndex << "\n"
        << "      FxFixed      : " << c.fxFixed << "\n"
        << "      TOAFixed     : " << c.toaFixed << "\n"
        << "      SRPFixed     : " << c.srpFixed << "\n"
        << "      SginalNormal : " << c.signalNormal << "\n"
        << "      Polarization:: \n"
        << c.polarization << "\n"
        << "      FxC          : " << c.fxC << "\n"
        << "      FxBW         : " << c.fxBW << "\n"
        << "      FxBWNoise    : " << c.fxBWNoise << "\n"
        << "      TOASaved     : " << c.toaSaved << "\n";
    if (c.toaExtended.get())
    {
        os << *(c.toaExtended) << "\n";
    }
    os << c.dwellTimes << "\n"
        << c.imageArea << "\n"
        << "      Antenna:: \n"
        << "      TxAPCId      : " << c.antenna->txAPCId << "\n"
        << "      TxAPATId     : " << c.antenna->txAPATId << "\n"
        << "      RcvAPCId     : " << c.antenna->rcvAPCId << "\n"
        << "      RcvAPATId    : " << c.antenna->rcvAPATId << "\n"
        << "      TxRcv:: \n";
    for (size_t ii = 0; ii < c.txRcv->txWFId.size(); ++ii)
    {
        os << "      TxWFId       : " << c.txRcv->txWFId[ii] << "\n";
    }
    for (size_t ii = 0; ii < c.txRcv->rcvId.size(); ++ii)
    {
        os << "      RcvId       : " << c.txRcv->rcvId[ii] << "\n";
    }
    if (c.tgtRefLevel.get())
    {
        os << *(c.tgtRefLevel) << "\n";
    }
    if (c.noiseLevel.get())
    {
        os << *(c.noiseLevel) << "\n";
    }
    return os;
}

std::ostream& operator<< (std::ostream& os, const Channel& c)
{
    os << "Channel:: \n"
        << "  RefChId          : " << c.refChId << "\n"
        << "  FxFixedCphd      : " << c.fxFixedCphd << "\n"
        << "  TOAFixedCphd     : " << c.toaFixedCphd << "\n"
        << "  SRPFixedCphd     : " << c.srpFixedCphd << "\n"
        << "  Parameters:: \n";
    for (size_t ii = 0; ii < c.parameters.size(); ++ii)
    {
        os << c.parameters[ii] << "\n";
    }
    for (size_t ii = 0; ii < c.addedParameters.size(); ++ii)
    {
        os << "  Parameter name   : " << c.addedParameters[ii].getName() << "\n";
        os << "  Parameter value   : " << c.addedParameters[ii].str() << "\n";
    }
    return os;
}
}

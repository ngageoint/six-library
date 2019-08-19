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
#include <six/Init.h>

namespace cphd
{

TOAExtended::TOAExtended() :
    toaExtSaved(six::Init::undefined<double>())
    {}

TOAExtended::LFMEclipse::LFMEclipse() :
    fxEarlyLow(six::Init::undefined<double>()),
    fxEarlyHigh(six::Init::undefined<double>()),
    fxLateLow(six::Init::undefined<double>()),
    fxLateHigh(six::Init::undefined<double>())
{}

DwellTimes::DwellTimes() :
    codId(six::Init::undefined<std::string>()),
    dwellId(six::Init::undefined<std::string>())
{}

TgtRefLevel::TgtRefLevel() :
    ptRef(six::Init::undefined<double>())
{}

Point::Point() :
    fx(six::Init::undefined<double>()),
    pn(six::Init::undefined<double>())
{}

NoiseLevel::NoiseLevel() :
    pnRef(six::Init::undefined<double>()),
    bnRef(six::Init::undefined<double>())
{}

ChannelParameter::ChannelParameter() :
    identifier(six::Init::undefined<std::string>()),
    refVectorIndex(six::Init::undefined<size_t>()),
    fxFixed(six::Init::undefined<six::BooleanType>()),
    toaFixed(six::Init::undefined<six::BooleanType>()),
    srpFixed(six::Init::undefined<six::BooleanType>()),
    signalNormal(six::Init::undefined<six::BooleanType>()),
    fxC(six::Init::undefined<double>()),
    fxBW(six::Init::undefined<double>()),
    fxBWNoise(six::Init::undefined<double>()),
    toaSaved(six::Init::undefined<double>())
{}

ChannelParameter::Antenna::Antenna() :
    txAPCId(six::Init::undefined<std::string>()),
    txAPATId(six::Init::undefined<std::string>()),
    rcvAPCId(six::Init::undefined<std::string>()),
    rcvAPATId(six::Init::undefined<std::string>())
{}

Channel::Channel() :
    refChId(six::Init::undefined<std::string>()),
    fxFixedCphd(six::Init::undefined<six::BooleanType>()),
    toaFixedCphd(six::Init::undefined<six::BooleanType>()),
    srpFixedCphd(six::Init::undefined<six::BooleanType>())
{}


}

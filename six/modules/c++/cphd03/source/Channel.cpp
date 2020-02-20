/* =========================================================================
 * This file is part of cphd03-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * cphd03-c++ is free software; you can redistribute it and/or modify
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

#include <six/Init.h>
#include <cphd03/Channel.h>

namespace cphd03
{
ChannelParameters::ChannelParameters() :
    srpIndex(0),
    nomTOARateSF(0.0),
    fxCtrNom(0.0),
    bwSavedNom(0.0),
    toaSavedNom(0.0),
    txAntIndex(six::Init::undefined<size_t>()),
    rcvAntIndex(six::Init::undefined<size_t>()),
    twAntIndex(six::Init::undefined<size_t>())
{
}

std::ostream& operator<< (std::ostream& os, const ChannelParameters& d)
{
    os << "ChannelParameters::\n"
       << "  srpIndex    : " << d.srpIndex << "\n"
       << "  nomTOARateSF: " << d.nomTOARateSF << "\n"
       << "  fxCtrNom    : " << d.fxCtrNom<< "\n"
       << "  bwSavedNom  : " << d.bwSavedNom<< "\n"
       << "  toaSavedNom : " << d.toaSavedNom << "\n";

    if (!six::Init::isUndefined(d.txAntIndex))
    {
        os << "  txAntIndex : " << d.txAntIndex << "\n";
    }

    if (!six::Init::isUndefined(d.rcvAntIndex))
    {
        os << "  rcvAntIndex: " << d.rcvAntIndex << "\n";
    }
    if (!six::Init::isUndefined(d.twAntIndex))
    {
        os << "  twAntIndex : " << d.twAntIndex << "\n";
    }

    return os;
}

std::ostream& operator<< (std::ostream& os, const Channel& d)
{
    os << "Channel::\n";
    for (size_t ii = 0; ii < d.parameters.size(); ++ii)
    {
        os << "  [" << (ii + 1) << "] Parameters: " << d.parameters[ii] << "\n";
    }

    return os;
}
}

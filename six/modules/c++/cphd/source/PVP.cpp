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

#include <cphd/PVP.h>
#include <six/Init.h>

namespace cphd
{

PVPType::PVPType() :
    size(0),
    offset(six::Init::undefined<size_t>()),
    format(six::Init::undefined<std::string>())
{}

APVPType::APVPType() :
    name(six::Init::undefined<std::string>())
{}




std::ostream& operator<< (std::ostream& os, const PVPType& p)
{
    os << "    Size           : " << p.size << "\n"
        << "    Offset         : " << p.offset << "\n"
        << "    Format         : " << p.format << "\n";
    return os;
}

std::ostream& operator<< (std::ostream& os, const APVPType& a)
{
    os << "    Name           : " << a.name << "\n"
        << (PVPType)a;

    return os;
}

std::ostream& operator<< (std::ostream& os, const Pvp& p)
{
    os << "PVP:: \n"
        << "  TxTime           : " << p.txTime << "\n"
        << "  TxPos            : " << p.txPos  << "\n"
        << "  TxVel            : " << p.txVel  << "\n"
        << "  RcvTime          : " << p.rcvTime << "\n"
        << "  RcvPos           : " << p.rcvPos << "\n"
        << "  RcvVel           : " << p.rcvVel << "\n"
        << "  SRPPos           : " << p.srpPos << "\n"
        << "  AmpSF            : " << p.ampSF << "\n"
        << "  aFDOP            : " << p.aFDOP << "\n"
        << "  aFRR1            : " << p.aFRR1 << "\n"
        << "  aFRR2            : " << p.aFRR2 << "\n"
        << "  FX1              : " << p.fx1 << "\n"
        << "  FX2              : " << p.fx2 << "\n"
        << "  FXN1             : " << p.fxN1 << "\n"
        << "  FXN2             : " << p.fxN2 << "\n"
        << "  TOA1             : " << p.toa1 << "\n"
        << "  TOA2             : " << p.toa2 << "\n"
        << "  TOAE1            : " << p.toaE1 << "\n"
        << "  TOAE2            : " << p.toaE2 << "\n"
        << "  TDTropoSRP       : " << p.tdTropoSRP << "\n"
        << "  TDIonoSRP        : " << p.tdIonoSRP << "\n"
        << "  SC0              : " << p.sc0 << "\n"
        << "  SCSS              : " << p.scSS << "\n"
        << "  SIGNAL              : " << p.signal << "\n";

    for (size_t i = 0; i < p.addedPVP.size(); ++i)
    {
        os << "  AddedPVP:: \n"
            << p.addedPVP[i] << "\n";
    }
    return os;
}


}

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
#include <six/Init.h>
#include <six/Utilities.h>
#include <cphd/Global.h>

namespace cphd
{
Global::Global()
{
}

Timeline::Timeline() :
    collectionStart(six::Init::undefined<DateTime>()),
    rcvCollectionStart(six::Init::undefined<DateTime>()),
    txTime1(0.0),
    txTime2(0.0)
{
}

FxBand::FxBand() :
    fxMin(0.0),
    fxMax(0.0)
{
}

TOASwath::TOASwath() :
    toaMin(0.0),
    toaMax(0.0)
{
}

TropoParameters::TropoParameters() :
    n0(0.0)
{
}

IonoParameters::IonoParameters() :
    tecv(0.0),
    f2Height(six::Init::undefined<size_t>())
{
}

bool Global::operator==(const Global& other) const
{
    return domainType == other.domainType &&
           sgn == other.sgn &&
           timeline == other.timeline &&
           fxBand == other.fxBand &&
           toaSwath == other.toaSwath &&
           tropoParameters == other.tropoParameters &&
           ionoParameters == other.ionoParameters;
}


std::ostream& operator<< (std::ostream& os, const Global& d)
{
    os << "Global::\n"
       << "  domainType       : " << d.domainType.toString() << "\n"
       << "  SGN              : " << d.sgn.toString() << "\n"
       << "  timeline         : " << d.timeline << "\n"
       << "  fxBand           : " << d.fxBand << "\n"
       << "  toaSwath         : " << d.toaSwath << "\n";
    if (d.tropoParameters.get())
    {
        os << "  tropoParameters  : " << *d.tropoParameters << "\n";
    }
    if (d.ionoParameters.get())
    {
        os << " ionoParameters    : " << *d.ionoParameters << "\n";
    }
    return os;
}

std::ostream& operator<< (std::ostream& os, const Timeline& d)
{
    os << "Timeline::\n"
       << "  collectionStart       : "
       << six::toString(d.collectionStart) << "\n";
    if (!six::Init::isUndefined(d.rcvCollectionStart))
    {
        os << "  rcvCollectionStart    : "
           << six::toString(d.rcvCollectionStart) << "\n";
    }
    os << "  txTime1               : " << d.txTime1 << "\n"
       << "  txTime2               : " << d.txTime2 << "\n";
    return os;
}

std::ostream& operator<< (std::ostream& os, const FxBand& d)
{
    os << "FxBand::\n"
       << "  fxMin                 : " << d.fxMin << "\n"
       << "  fxMax                 : " << d.fxMax << "\n";
    return os;
}

std::ostream& operator<< (std::ostream& os, const TOASwath& d)
{
    os << "TOASwath::\n"
       << "  toaMin                : " << d.toaMin << "\n"
       << "  toaMax                : " << d.toaMax << "\n";
    return os;
}

std::ostream& operator<< (std::ostream& os, const TropoParameters& d)
{
    os << "TropoParameters::\n"
       << "  n0                    : " << d.n0<< "\n"
       << "  refHeight             : " << d.refHeight << "\n";
    return os;
}

std::ostream& operator<< (std::ostream& os, const IonoParameters& d)
{
    os << "IonoParameters::\n"
       << "  tecv                  : " << d.tecv << "\n"
       << "  f2Height              : " << d.f2Height << "\n";
    return os;
}
}

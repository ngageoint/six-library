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
#include <cphd03/VectorParameters.h>

namespace cphd03
{
FxParameters::FxParameters() :
    Fx0(0),
    FxSS(0),
    Fx1(0),
    Fx2(0)
{
}

std::ostream& operator<< (std::ostream& os, const FxParameters& d)
{
    os << "FxParameters::" << "\n"
       << "  Fx0  : " << d.Fx0 << "\n"
       << "  Fx_SS: " << d.FxSS << "\n"
       << "  Fx1  : " << d.Fx1 << "\n"
       << "  Fx2  : " << d.Fx2 << "\n";
    return os;
}

TOAParameters::TOAParameters() :
    deltaTOA0(0),
    toaSS(0)
{
}

std::ostream& operator<< (std::ostream& os, const TOAParameters& d)
{
    os << "TOAParameters::\n"
       << "  DeltaTOA0: " << d.deltaTOA0 << "\n"
       << "  TOA_SS   : " << d.toaSS << "\n";
     return os;
}

VectorParameters::VectorParameters() :
    txTime(0),
    txPos(0),
    rcvTime(0),
    rcvPos(0),
    srpTime (six::Init::undefined<ptrdiff_t>()),
    srpPos(0),
    tropoSRP(six::Init::undefined<ptrdiff_t>()),
    ampSF(six::Init::undefined<ptrdiff_t>())
{
}

ptrdiff_t VectorParameters::txTimeOffset() const
{
    return 0;
}

ptrdiff_t VectorParameters::txPosOffset() const
{
    return (txTimeOffset() + txTime);
}

ptrdiff_t VectorParameters::rcvTimeOffset() const
{
    return (txPosOffset() + txPos);
}

ptrdiff_t VectorParameters::rcvPosOffset() const
{
    return (rcvTimeOffset() + rcvTime);
}

ptrdiff_t VectorParameters::srpTimeOffset() const
{
    const ptrdiff_t pos = six::Init::isUndefined(srpTime) ?
            -1 : rcvPosOffset() + rcvPos;
    return pos;
}

ptrdiff_t VectorParameters::srpPosOffset() const
{
    ptrdiff_t pos = rcvPosOffset() + rcvPos;
    if (!six::Init::isUndefined(srpTime))
    {
        pos += srpTime;
    }
    return pos;
}

ptrdiff_t VectorParameters::tropoSRPOffset() const
{
    const ptrdiff_t pos = six::Init::isUndefined(tropoSRP) ?
            -1 : (srpPosOffset() + srpPos);
    return pos;
}

ptrdiff_t VectorParameters::ampSFOffset() const
{
    ptrdiff_t pos = -1;
    if (!six::Init::isUndefined(ampSF))
    {
        pos = srpPosOffset() + srpPos;
        if (!six::Init::isUndefined(tropoSRP))
        {
            pos += tropoSRP;
        }
    }
    return pos;
}

ptrdiff_t VectorParameters::Fx0Offset() const
{
    ptrdiff_t pos = -1;
    if (fxParameters.get() != NULL)
    {
        pos = srpPosOffset() + srpPos;
        if (!six::Init::isUndefined(tropoSRP))
        {
            pos += tropoSRP;
        }
        if (!six::Init::isUndefined(ampSF))
        {
            pos += ampSF;
        }
    }
    return pos;
}

ptrdiff_t VectorParameters::FxSSOffset() const
{
    ptrdiff_t pos = Fx0Offset();
    if (pos != -1)
    {
        pos += fxParameters->Fx0;
    }
    return pos;
}

ptrdiff_t VectorParameters::Fx1Offset() const
{
    ptrdiff_t pos = FxSSOffset();
    if (pos != -1)
    {
        pos += fxParameters->FxSS;
    }
    return pos;
}

ptrdiff_t VectorParameters::Fx2Offset() const
{
    ptrdiff_t pos = Fx1Offset();
    if (pos != -1)
    {
        pos += fxParameters->Fx1;
    }
    return pos;
}

ptrdiff_t VectorParameters::deltaTOA0Offset() const
{
    ptrdiff_t pos = -1;
    if (toaParameters.get() != NULL)
    {
        pos = srpPosOffset() + srpPos;
        if (!six::Init::isUndefined(tropoSRP))
        {
            pos += tropoSRP;
        }
        if (!six::Init::isUndefined(ampSF))
        {
            pos += ampSF;
        }
    }
    return pos;
}

ptrdiff_t VectorParameters::toaSSOffset() const
{
    ptrdiff_t pos = deltaTOA0Offset();
    if (pos != -1)
    {
        pos += toaParameters->deltaTOA0;
    }
    return pos;
}

bool VectorParameters::operator==(const VectorParameters& other) const
{
    if (txTime != other.txTime ||
        txPos != other.txPos ||
        rcvTime != other.rcvTime ||
        rcvPos != other.rcvPos ||
        srpTime != other.srpTime ||
        srpPos != other.srpPos ||
        tropoSRP != other.tropoSRP ||
        ampSF != other.ampSF)
    {
        return false;
    }

    if (fxParameters.get() && other.fxParameters.get())
    {
        if (*fxParameters != *other.fxParameters)
        {
            return false;
        }
    }
    else if (fxParameters.get() || other.fxParameters.get())
    {
        return false;
    }

    if (toaParameters.get() && other.toaParameters.get())
    {
        if (*toaParameters != *other.toaParameters)
        {
            return false;
        }
    }
    else if (toaParameters.get() || other.toaParameters.get())
    {
        return false;
    }

    return true;
}

std::ostream& operator<< (std::ostream& os, const VectorParameters& d)
{
    os << "VectorParameters::" << "\n"
       << "  TxTime  : " << d.txTime << "\n"
       << "  TxPos   : " << d.txPos << "\n"
       << "  RcvTime : " << d.rcvTime << "\n"
       << "  RcvPos  : " << d.rcvPos << "\n";
    if (!six::Init::isUndefined(d.srpTime))
    {
        os << "  SRPTime : " << d.srpTime << "\n";
    }
    os << "  SRPPos  : " << d.srpPos << "\n";
    if (!six::Init::isUndefined(d.tropoSRP))
    {
        os << "  TropoSRP: " << d.tropoSRP << "\n";
    }
    if (!six::Init::isUndefined(d.ampSF))
    {
        os << "  AmpSF   : " << d.ampSF << "\n";
    }

    if (d.fxParameters.get())
    {
        os << "  " << *d.fxParameters << "\n";
    }
    if (d.toaParameters.get())
    {
        os << "  " << *d.toaParameters << "\n";
    }

    return os;
}
}

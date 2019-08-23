/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2018, MDA Information Systems LLC
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


#ifndef __CPHD_GLOBAL_H__
#define __CPHD_GLOBAL_H__

#include <ostream>

#include <mem/ScopedCopyablePtr.h>
#include <six/sicd/RadarCollection.h>
#include <cphd/Enums.h>
#include <cphd/Types.h>

namespace cphd
{
struct Timeline
{
    Timeline();

    bool operator==(const Timeline& other) const
    {
        return collectionStart == other.collectionStart &&
            rcvCollectionStart == other.rcvCollectionStart &&
            txTime1 == other.txTime1 &&
            txTime2 == other.txTime2;
    }

    bool operator!=(const Timeline& other) const
    {
        return !((*this) == other);
    }

    DateTime collectionStart;
    DateTime rcvCollectionStart;
    double txTime1;
    double txTime2;
};

struct FxBand
{
    FxBand();

    bool operator==(const FxBand& other) const
    {
        return fxMin == other.fxMin &&
            fxMax == other.fxMax;
    }

    bool operator!=(const FxBand& other) const
    {
        return !((*this) == other);
    }

    double fxMin;
    double fxMax;
};

struct TOASwath
{
    TOASwath();

    bool operator==(const TOASwath& other) const
    {
        return toaMin == other.toaMin &&
            toaMax == other.toaMax;
    }

    bool operator!=(const TOASwath& other) const
    {
        return !((*this) == other);
    }

    double toaMin;
    double toaMax;
};

struct TropoParameters
{
    TropoParameters();

    bool operator==(const TropoParameters& other) const
    {
        return n0 == other.n0 && refHeight == other.refHeight;
    }

    bool operator!=(const TropoParameters& other) const
    {
        return !((*this) == other);
    }

    double n0;
    RefHeight refHeight;
};

struct IonoParameters
{
    IonoParameters();

    bool operator==(const IonoParameters& other) const
    {
        return tecv == other.tecv && f2Height == other.f2Height;
    }

    bool operator!=(const IonoParameters& other) const
    {
        return !((*this) == other);
    }

    double tecv;
    double f2Height;
};


struct Global
{
    Global();

    bool operator==(const Global& other) const
    {
        return domainType == other.domainType &&
               sgn == other.sgn &&
               timeline == other.timeline &&
               fxBand == other.fxBand &&
               toaSwath == other.toaSwath &&
               tropoParameters == other.tropoParameters &&
               ionoParameters == other.ionoParameters;
    }

    bool operator !=(const Global& other) const
    {
        return !((*this) == other);
    }

    DomainType domainType;
    PhaseSGN sgn;
    Timeline timeline;
    FxBand fxBand;
    TOASwath toaSwath;
    mem::ScopedCopyablePtr<TropoParameters> tropoParameters;
    mem::ScopedCopyablePtr<IonoParameters> ionoParameters;
};

std::ostream& operator<< (std::ostream& os, const Global& d);
std::ostream& operator<< (std::ostream& os, const Timeline& d);
std::ostream& operator<< (std::ostream& os, const FxBand& d);
std::ostream& operator<< (std::ostream& os, const TOASwath& d);
std::ostream& operator<< (std::ostream& os, const TropoParameters& d);
std::ostream& operator<< (std::ostream& os, const IonoParameters& d);
}

#endif

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


#ifndef __CPHD_DWELL_H__
#define __CPHD_DWELL_H__

#include <ostream>
#include<vector>

#include <mem/ScopedCopyablePtr.h>
#include <cphd/Enums.h>
#include <cphd/Types.h>

namespace cphd
{

struct DwellTime 
{
    DwellTime();

    bool operator==(const DwellTime& other) const
    {
        return identifier == other.identifier && dwellTimePoly == other.dwellTimePoly;
    }

    bool operator!=(const DwellTime& other) const
    {
        return !((*this) == other);
    }


    std::string identifier;
    Poly2D dwellTimePoly;
};

struct COD
{
    COD();

    bool operator==(const COD& other) const
    {
        return identifier == other.identifier && codTimePoly == other.codTimePoly;
    }

    bool operator!=(const COD& other) const
    {
        return !((*this) == other);
    }

    std::string identifier;
    Poly2D codTimePoly;
};

struct Dwell
{
    Dwell();

    bool operator==(const Dwell& other) const
    {
        return numCODTimes == other.numCODTimes && numDwellTimes == other.numDwellTimes &&
                cod == other.cod && dtime == other.dtime;
    }

    bool operator!=(const Dwell& other) const
    {
        return !((*this) == other);
    }

    size_t numCODTimes;
    size_t numDwellTimes;
    std::vector<COD> cod;
    std::vector<DwellTime> dtime;
};

std::ostream& operator<< (std::ostream& os, const DwellTime& d);
std::ostream& operator<< (std::ostream& os, const COD& c);
std::ostream& operator<< (std::ostream& os, const Dwell& d);
}

#endif

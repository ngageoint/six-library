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

#include <cphd03/Utilities.h>
#include <cphd03/Antenna.h>

namespace cphd03
{
Antenna::Antenna() :
    numTxAnt(0),
    numRcvAnt(0),
    numTWAnt(0)
{
}

std::ostream& operator<< (std::ostream& os, const Antenna& d)
{
    os << "Antenna::\n"
       << "  numTxAnt : " << d.numTxAnt << "\n"
       << "  numRcvAnt: " << d.numRcvAnt << "\n"
       << "  numTWAnt : " << d.numTWAnt << "\n";

    for (size_t ii = 0; ii < d.tx.size(); ++ii)
    {
         os << "  [" << (ii + 1) << "] Tx:\n  " << d.tx[ii] << "\n";
    }

    for (size_t ii = 0; ii < d.rcv.size(); ++ii)
    {
        os << "  [" << (ii + 1) << "] Rcv:\n  " << d.rcv[ii] << "\n";
    }

    for (size_t ii = 0; ii < d.twoWay.size(); ++ii)
    {
        os << "  [" << (ii + 1) << "] TwoWay:\n  " << d.twoWay[ii] << "\n";
    }

    return os;
}
}

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
#include <cphd03/SRP.h>
#include <six/Utilities.h>

namespace cphd03
{
SRP::SRP() :
    numSRPs(0)
{
}

std::ostream& operator<< (std::ostream& os, const SRP& d)
{
    os << "SRP::" << "\n"
       << "  SRPType : " << d.srpType << "\n"
       << "  NumSRPs : " << d.numSRPs  << "\n";

    for (size_t ii = 0; ii < d.srpPT.size(); ++ii)
    {
        os << "  [" << (ii + 1) << "] SRPPT      : "
           <<six::toString(d.srpPT[ii]) << "\n";
    }

    for (size_t ii = 0; ii < d.srpPVTPoly.size(); ++ii)
    {
        os << "  [" << (ii + 1) << "] SRPPVTPoly :\n"
           << six::toString(d.srpPVTPoly[ii]) << "\n";
    }

    for (size_t ii = 0; ii < d.srpPVVPoly.size(); ++ii)
    {
        os << "  [" << (ii + 1) << "] SRPPVVPoly :\n"
           << six::toString(d.srpPVVPoly[ii]) << "\n";
    }
    return os;
}
}

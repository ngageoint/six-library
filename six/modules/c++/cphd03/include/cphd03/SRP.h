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

#pragma once
#ifndef __CPHD03_SRP_H__
#define __CPHD03_SRP_H__

#include <iostream>
#include <cphd/Types.h>
#include <cphd03/Utilities.h>
#include <cphd03/Exports.h>
#include <six/Init.h>


namespace cphd03
{
struct SIX_CPHD03_API SRP final
{
    SRP();

    bool operator==(const SRP& other) const
    {
        return srpType == other.srpType &&
               numSRPs == other.numSRPs &&
               srpPT == other.srpPT &&
               srpPVTPoly == other.srpPVTPoly &&
               srpPVVPoly == other.srpPVVPoly;
    }

    bool operator!=(const SRP& other) const
    {
        return !((*this) == other);
    }

    //! Type can be FIXEDPT, PVTPOLY, PVVPOLY, STEPPED
    cphd::SRPType srpType;

    //! Can be 1 to the number of channels
    //! Will be zero if SRPType is STEPPED ??
    size_t numSRPs;

    //! Conditional, when srpType is FIXEDPT
    std::vector<cphd::Vector3> srpPT;

     //! Conditional, when srpType is PVTPOLY
    std::vector<cphd::PolyXYZ> srpPVTPoly;

    //! Conditional, when srpType is PVVPOLY
    std::vector<cphd::PolyXYZ> srpPVVPoly;
};

std::ostream& operator<< (std::ostream& os, const SRP& d);
}

#endif

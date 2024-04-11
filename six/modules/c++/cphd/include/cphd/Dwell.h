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
#pragma once
#ifndef __CPHD_DWELL_H__
#define __CPHD_DWELL_H__

#include <ostream>
#include<vector>

#include <cphd/Enums.h>
#include <cphd/Types.h>
#include <cphd/Exports.h>

#include <mem/ScopedCopyablePtr.h>

namespace cphd
{

/*
 *  \struct DwellTime
 *
 *  \brief Dwell Time Polynomial
 */
struct SIX_CPHD_API DwellTime
{
    //! Constructor
    DwellTime();

    //! Equality operators
    bool operator==(const DwellTime& other) const
    {
        return identifier == other.identifier && dwellTimePoly == other.dwellTimePoly;
    }
    bool operator!=(const DwellTime& other) const
    {
        return !((*this) == other);
    }

    //! String that uniquely identifies this Dwell Time polynomial (Dwell_ID)
    std::string identifier;

    //! Polynomial that yields Dwell Time as a function of IAX and IAY
    Poly2D dwellTimePoly;
};

/*
 *  \struct COD
 *
 *  \brief Center of Dwell Time Polynomial
 */
struct SIX_CPHD_API COD
{
    //! Constructor
    COD();

    //! Equality operators
    bool operator==(const COD& other) const
    {
        return identifier == other.identifier && codTimePoly == other.codTimePoly;
    }
    bool operator!=(const COD& other) const
    {
        return !((*this) == other);
    }

    //! String that uniquely identifies this COD Time polynomial (COD_ID)
    std::string identifier;

    //! Polynomial that yields COD Time as a function of IAX and IAY
    Poly2D codTimePoly;
};

/*
 *  \struct Dwell
 *
 *  \brief Dwell Time parameters
 *
 *  Parameters that specify the dwell time supported by the signal arrays
 *  contained in the CPHD product
 *  See section 7.3
 */
struct SIX_CPHD_API Dwell
{
    //! Constructor
    Dwell();

    // Equality operator
    bool operator==(const Dwell& other) const
    {
        return cod == other.cod && dtime == other.dtime;
    }
    bool operator!=(const Dwell& other) const
    {
        return !((*this) == other);
    }

    //! Center of Dwell (COD) Time Polynomial
    std::vector<COD> cod;

    //! Dwell Time Polynomial
    std::vector<DwellTime> dtime;
};

//! Ostream operators
std::ostream& operator<< (std::ostream& os, const DwellTime& d);
std::ostream& operator<< (std::ostream& os, const COD& c);
std::ostream& operator<< (std::ostream& os, const Dwell& d);
}

#endif

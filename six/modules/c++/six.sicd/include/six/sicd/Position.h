/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SIX_POSITION_H__
#define __SIX_POSITION_H__

#include <logging/Logger.h>
#include <mem/ScopedCopyablePtr.h>
#include "six/Types.h"
#include "six/Init.h"
#include "six/Parameter.h"

namespace six
{
namespace sicd
{
struct SCPCOA;

/*!
 *  \struct RcvAPC
 *  \brief SICD RcvAPC
 *
 *  Receive aperture phase center (APC).  The size of the vector
 *  of the vector the number of receive APC polys
 */
struct RcvAPC
{
    //!  Constructor
    RcvAPC()
    {
    }

    std::vector<PolyXYZ> rcvAPCPolys;
    
    //! Equality operator
    bool operator==(const RcvAPC& rhs) const
    {
        return rcvAPCPolys == rhs.rcvAPCPolys;
    }

    bool operator!=(const RcvAPC& rhs) const
    {
        return !(*this == rhs);
    }
};

/*!
 *  \struct Position
 *  \brief SICD Position
 *
 *  This block describes the platform and ground reference
 *  positions vs. time.
 */
struct Position
{
    /*!
     *  Aperture reference point as a function of time in ECEF meters
     *  t = 0 at collection start
     */
    PolyXYZ arpPoly;

    /*!
     *  Ground reference point as a function of time in ECEF meters.
     *  t = 0 at collection start
     */
    PolyXYZ grpPoly;

    /*!
     *  Transmit aperture phase center (APC) in ECEF as a function of time
     *  t = 0 at collection start.
     *  Always included for bistatic collections
     */
    PolyXYZ txAPCPoly;

    /*!
     *  (Optional) Receive aperture phase center.
     *
     */
    mem::ScopedCopyablePtr<RcvAPC> rcvAPC;
    
    //!  Constructor
    Position();

    //! Equality operator
    bool operator==(const Position& rhs) const
    {
        return (arpPoly == rhs.arpPoly && grpPoly == rhs.grpPoly &&
            txAPCPoly == rhs.txAPCPoly && rcvAPC == rhs.rcvAPC);
    }

    bool operator!=(const Position& rhs) const
    {
        return !(*this == rhs);
    }

    void fillDerivedFields(const SCPCOA& scpcoa);
    bool validate(logging::Logger& log) const;
};

}
}
#endif


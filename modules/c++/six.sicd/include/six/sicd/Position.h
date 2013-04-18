/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2013, General Dynamics - Advanced Information Systems
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

#include "six/Types.h"
#include "six/Init.h"
#include "six/Parameter.h"

namespace six
{
namespace sicd
{
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

    //!  Destructor
    ~RcvAPC()
    {
    }
    //!  Clone this object
    RcvAPC* clone() const
    {
        return new RcvAPC(*this);
    }

    std::vector<PolyXYZ> rcvAPCPolys;
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
    RcvAPC* rcvAPC;
    
    //!  Constructor, optional rcvAPC set to NULL
    Position() :
        rcvAPC(NULL)
    {
	// Initialize other optional elements
        arpPoly = Init::undefined<PolyXYZ>();
    	grpPoly = Init::undefined<PolyXYZ>();
    	txAPCPoly = Init::undefined<PolyXYZ>();
    }
    //!  Destroy, including rcvAPC if not NULL
    ~Position();

    //!  Copy, including rcvAPC if not NULL
    Position* clone() const;

};

}
}
#endif


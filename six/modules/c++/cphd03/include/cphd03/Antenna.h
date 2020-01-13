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

#ifndef __CPHD03_ANTENNA_H__
#define __CPHD03_ANTENNA_H__

#include <ostream>
#include <vector>

#include <six/sicd/Antenna.h>

namespace cphd03
{
/*!
 *  \struct ElectricalBoresight
 *  \brief CPHD EB Parameter
 *
 *  Electrical boresight steering directions
 *  the DCXPoly is the EB steering x-axis direction
 *  cosine (DCX) as a function of slow time
 */
typedef six::sicd::ElectricalBoresight ElectricalBoresight;

/*!
 *  \struct HalfPowerBeamwidths
 *  \brief CPHD HPBW Parameter
 *
 *  Half-power beamwidths.  For electronically steered
 *  arrays, the EB is steered to DCX = 0, DCY = 0
 */
typedef six::sicd::HalfPowerBeamwidths HalfPowerBeamwidths;

/*!
 *  \struct GainAndPhasePolys
 *  \brief CPHD Array & Elem Parameter
 *
 *  Mainlobe array pattern polynomials.  For
 *  electronically steered arrays, the EB is steered to
 *  DCX = 0, DCY = 0.
 */
typedef six::sicd::GainAndPhasePolys GainAndPhasePolys;

/*!
 *  \struct AntennaParameters
 *  \brief CPHD Tx/Rcv, etc Antenna parameter
 *
 */
typedef six::sicd::AntennaParameters AntennaParameters;

/*!
 *  \struct Antenna
 *  \brief CPHD Antenna parameter
 *
 *  Parameters that describe the antenna(s) during collection.
 *  Parameters describe the antenna orientation, pointing direction
 *  and beamshape during the collection.
 *
 *  There can be a as many sets of antenna parameters as there are
 *  channels in the collection
 *  Parameters may be provided separably for the transmit (Tx) antenna
 *  and the receieve (Rcv) antenna.  A single set of prarameters may
 *  be provided for a combined two-way pattern (as appropriate)
 */
 struct Antenna
{
    //!  Constructor
    Antenna();

    bool operator==(const Antenna& other) const
    {
        return numTxAnt == other.numTxAnt &&
               numRcvAnt == other.numRcvAnt &&
               numTWAnt == other.numTWAnt &&
               tx == other.tx &&
               rcv == other.rcv &&
               twoWay == other.twoWay;
    }

    bool operator!=(const Antenna& other) const
    {
        return !((*this) == other);
    }

    //! Number of Transmit Antennae - up to one per channel
    size_t numTxAnt;

    //! Number of Receive Antennae - up to one per channel
    size_t numRcvAnt;

    //! Number of Two-Way Antennae - up to one per channel
    size_t numTWAnt;

    //! Transmit parameters
    std::vector<AntennaParameters> tx;

    //! Receive parameters
    std::vector<AntennaParameters> rcv;

    //! Two way parameters
    std::vector<AntennaParameters> twoWay;
};

std::ostream& operator<< (std::ostream& os, const Antenna& d);
}

#endif

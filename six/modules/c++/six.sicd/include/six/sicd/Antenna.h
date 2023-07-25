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

#pragma once
#ifndef SIX_six_sicd_Antenna_h_INCLUDED_
#define SIX_six_sicd_Antenna_h_INCLUDED_

#include <ostream>
#include <std/optional>

#include "six/Types.h"
#include "six/Init.h"
#include "six/Parameter.h"

#include <mem/ScopedCopyablePtr.h>

namespace six
{
namespace sicd
{
/*!
 *  \struct ElectricalBoresight
 *  \brief SICD EB Parameter
 *
 *  Electrical boresight steering directions
 *  the DCXPoly is the EB sterring x-axis direction
 *  cosine (DCX) as a function of slow time
 */
struct ElectricalBoresight final
{
    //! Constructor
    ElectricalBoresight();

    bool operator==(const ElectricalBoresight& other) const
    {
        return (dcxPoly == other.dcxPoly)
            && (dcyPoly == other.dcyPoly)
            && (useEBPVP == other.useEBPVP);
    }

    bool operator!=(const ElectricalBoresight& other) const
    {
        return !((*this) == other);
    }

    //! SICD DCXPoly
    Poly1D dcxPoly;

    //! SICD DCYPoly
    Poly1D dcyPoly;

    //! CPHD 1.1.0 UseEBPVP
    //! Indicates the provided EB PVP arrays provide a more accurate description the EB Steering vector vs. time.
    std::optional<bool> useEBPVP;
};

std::ostream& operator<< (std::ostream& os, const ElectricalBoresight& d);

/*!
 *  \struct HalfPowerBeamwidths
 *  \brief SICD HPBW Parameter
 *
 *  Half-power beamwidths.  For electronically steered
 *  arrays, the EB is steered to DCX = 0, DCY = 0
 *
 *  Removed in 1.0.0
 */
struct HalfPowerBeamwidths
{
    //! Constructor
    HalfPowerBeamwidths();

    bool operator==(const HalfPowerBeamwidths& other) const
    {
        return dcx == other.dcx &&
               dcy == other.dcy;
    }

    bool operator!=(const HalfPowerBeamwidths& other) const
    {
        return !((*this) == other);
    }

    //! SICD DCX
    double dcx;

    //! SICD DCY
    double dcy;
};

std::ostream& operator<< (std::ostream& os, const HalfPowerBeamwidths& d);

/*!
 *  \struct GainAndPhasePolys
 *  \brief SICD Array & Elem Parameter
 *
 *  Mainlobe array pattern polynomials.  For
 *  electronically steered arrays, the EB is steered to
 *  DCX = 0, DCY = 0.
 */
struct GainAndPhasePolys final
{
    //! No init right now, could do that and set const coef to zero
    GainAndPhasePolys();

    bool operator==(const GainAndPhasePolys& other) const
    {
        return (gainPoly == other.gainPoly)
            && (phasePoly == other.phasePoly)
            && (antGPId == other.antGPId);
    }

    bool operator!=(const GainAndPhasePolys& other) const
    {
        return !((*this) == other);
    }

    //! One way signal gain (in dB) as a function of DCX and DCY
    //! Gain relative to gain at DCX = 0, DCY = 0.  Const coeff = 0 always
    Poly2D gainPoly;

    //! One way signal phase (in cycles) as a function of DCX and DCY.
    //! Phase relative to phase at DCX = 0, DCY = 0. Const coeff = 0 always
    Poly2D phasePoly;

    //! Identifier of the Antenna Gain/Phase support array that specifies the one-way array pattern 
    std::optional<std::string> antGPId;  // new in CPHD 1.1.0
};

std::ostream& operator<< (std::ostream& os, const GainAndPhasePolys& d);

/*!
 *  \struct AntennaParameters
 *  \brief SICD Tx/Rcv, etc Antenna parameter
 *
 */
struct AntennaParameters
{

    //!  Constructor
    AntennaParameters();

    bool operator==(const AntennaParameters& other) const;

    bool operator!=(const AntennaParameters& other) const
    {
        return !((*this) == other);
    }

    //! Aperture X-axis direction in ECF as a function of time
    PolyXYZ xAxisPoly;

    //! Aperture Y-axis direction in ECF as a function of time
    PolyXYZ yAxisPoly;

    //! RF frequency for the HP beamwidths, array and element patterns
    //! and EB steering direction cosines
    double frequencyZero;

    //! Electrical boresight params
    mem::ScopedCopyablePtr<ElectricalBoresight> electricalBoresight;

    //! Half-power beamwidths
    //  Removed in 1.0.0
    mem::ScopedCopyablePtr<HalfPowerBeamwidths> halfPowerBeamwidths;

    /*!
     *  Mainlobe array pattern polynomials.  For
     *  electronically steered arrays, the EB is steered to
     *  DCX = 0 and DCY = 0
     *  field is optional in 0.4 and mandatory in 1.0
     */
    mem::ScopedCopyablePtr<GainAndPhasePolys> array;

    /*!
     *  Element array pattern polynomials for electronically
     *  steered arrays
     */
    mem::ScopedCopyablePtr<GainAndPhasePolys> element;

    /*!
     *  Gain polynomial (dB) vs. frequency for boresight
     *  at DCX = 0 and DCY = 0.  Frequency ratio input variable
     *  Const coeff = 0 always.
     */
    Poly1D gainBSPoly;

    // These are optional and are unset by default

    /*!
     *  Parameter indictating the EB shifts with frequency for an
     *  electronically steered array.  TS_FALSE = No shift with frequency
     *  TS_TRUE = Shift with frequency per ideal array theory
     */
    BooleanType electricalBoresightFrequencyShift;

    /*!
     *  Parameter indication the mainlobe width chainges with
     *  frequency.  FALSE = No change with frequency.  TRUE =
     *  Change with frequency per ideal array theory
     */
    BooleanType mainlobeFrequencyDilation;
};

std::ostream& operator<< (std::ostream& os, const AntennaParameters& d);

/*!
 *  \struct Antenna
 *  \brief SICD Antenna parameter
 *
 *  Parameters that describe the antenna(s) during collection.
 *  Parameters describe the antenna orientation, pointing direction
 *  and beamshape during the collection.
 *
 *  Parameters may be provided separably for the transmit (Tx) antenna
 *  and the receieve (Rcv) antenna.  A single set of prarameters may
 *  be provided for a combined two-way pattern (as appropriate)
 */
struct Antenna
{
    //! Transmit parameters
    mem::ScopedCopyablePtr<AntennaParameters> tx;

    //! Receieve parameters
    mem::ScopedCopyablePtr<AntennaParameters> rcv;

    //! Two way parameters
    mem::ScopedCopyablePtr<AntennaParameters> twoWay;

    //!  Constructor
    Antenna(){}

    //! Equality operator
    bool operator==(const Antenna& rhs) const
    {
        return (tx == rhs.tx && rcv == rhs.rcv && twoWay == rhs.twoWay);
    }

    bool operator!=(const Antenna& rhs) const
    {
        return !(*this == rhs);
    }
};

}
}
#endif // SIX_six_sicd_Antenna_h_INCLUDED_

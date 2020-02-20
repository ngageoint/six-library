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

#ifndef __CPHD_ANTENNA_H__
#define __CPHD_ANTENNA_H__

#include <ostream>
#include <vector>
#include <stddef.h>

#include <six/sicd/Antenna.h>

#include <cphd/Enums.h>
#include <cphd/Types.h>

namespace cphd
{
/*
 *  \struct AntCoordFrame
 *  \brief CPHD Antenna Coordinate Frame parameter
 *
 *  Unit vectors that describe the orientation of an
 *  Antenna Coordinate Frame as a function of time.
 *  AntCoordFrame used by CPHD, representing the tag
 *  <AntCoordFrame>.
 */
struct AntCoordFrame
{
    //! Constructor
    AntCoordFrame();

    //! Equality operators
    bool operator==(const AntCoordFrame& other) const
    {
        return identifier == other.identifier &&
                xAxisPoly == other.xAxisPoly &&
                yAxisPoly == other.yAxisPoly;
    }
    bool operator!=(const AntCoordFrame& other) const
    {
        return !((*this) == other);
    }

    //! String that uniquely identifies this ACF
    //! (ACF_ID)
    std::string identifier;

    //!Antenna coordinate frame X-Axis unit vector in
    //! ECF coordinates as a function time (sec).
    PolyXYZ xAxisPoly;

    //!Antenna coordinate frame Y-Axis unit vector in
    //! ECF coordinates as a function time (sec).
    PolyXYZ yAxisPoly;
};

/*
 *  \struct AntPhaseCenter
 *  \brief Parameters that describe each Antenna Phase Center.
 *
 *  AntPhaseCenter used by CPHD, representing the tag
 *  <AntPhaseCenter>.
 */
struct AntPhaseCenter
{
    //! Constructor
    AntPhaseCenter();

    //! Equality operators
    bool operator==(const AntPhaseCenter& other) const
    {
        return identifier == other.identifier &&
                acfId == other.acfId && apcXYZ == other.apcXYZ;
    }
    bool operator!=(const AntPhaseCenter& other) const
    {
        return !((*this) == other);
    }

    //! String that uniquely identifies this APC
    //! (APC_ID).
    std::string identifier;

    //! Identifier of Antenna Coordinate Frame used for
    //! computing the antenna gain and phase patterns
    std::string acfId;

    //! The APC location in the ACF XYZ coordinate
    //! frame
    Vector3 apcXYZ;
};

/*
 *  \struct AntPattern
 *  \brief CPHD Antenna Pattern parameter
 *
 *  Parameter set that defines each Antenna Pattern
 *  as a function of time.
 *  AntPattern used by CPHD, representing the tag
 *  <AntPattern>.
 */
struct AntPattern
{
    /*
     *  \struct GainPhaseArray
     *  \brief Gain Phase Array parameter
     *
     *  (Optional) Parameters that identify 2-D sampled Grain & Phase patterns at
     *  a single frequency value.
     *
     */
    struct GainPhaseArray
    {
        //! Constructor
        GainPhaseArray();

        //! Equality operators
        bool operator==(const GainPhaseArray& other) const
        {
            return freq == other.freq && arrayId == other.arrayId &&
                    elementId == other.elementId;
        }
        bool operator!=(const GainPhaseArray& other) const
        {
            return !((*this) == other);
        }

        //! Frequency value for which the sampled Array &
        //! Element pattern(s) are provided.
        double freq;

        //! Support Array identifier of the sampled
        //! Gain/Phase of the Array at RefFrequency
        std::string arrayId;

        //! (Optional) Support Array identifier of the sampled
        //! Gain/Phase of the Element at RefFrequency
        std::string elementId;
    };

    //! Default constructor
    AntPattern();

    //! Equality operators
    bool operator==(const AntPattern& other) const
    {
        return  identifier == other.identifier &&
                freqZero == other.freqZero && gainZero == other.gainZero &&
                ebFreqShift == other.ebFreqShift &&
                mlFreqDilation == other.mlFreqDilation &&
                gainBSPoly == other.gainBSPoly && eb == other.eb &&
                array == other.array && element == other.element &&
                gainPhaseArray == other.gainPhaseArray;
    }
    bool operator!=(const AntPattern& other) const
    {
        return !((*this) == other);
    }

    //! String that uniquely identifies this Antenna
    //! Pattern (APAT_ID).
    std::string identifier;

    //! The reference frequency (f_0) value for which
    //! the Electrical Boresight and array pattern
    //! polynomials are computed.
    double freqZero;

    //! (Optional) The reference antenna gain (G_0) at zero steering
    //! angle at the reference frequency (f_0)
    double gainZero;

    //! (Optional) Parameter to indicate the EB steering direction
    //! shifts with frequency.
    six::BooleanType ebFreqShift;

    //! (Optional) Parameter to indicate the mainlobe width varies
    //! with frequency.
    six::BooleanType mlFreqDilation;

    //! (Optional) Gain polynomial (dB) vs. frequency for the array
    //! pattern at boresight. Gain relative to gain at f_0.
    //! Poly1D gainBSPoly;
    Poly1D gainBSPoly;

    //! The Electrical Boresight steering direction versus
    //! time. Defines array pattern pointing direction
    six::sicd::ElectricalBoresight eb;

    //! Array Pattern polynomials that describe the
    //! mainlobe gain and phase patterns. Patterns
    //! defined at f = f_0
    six::sicd::GainAndPhasePolys array;

    //! Element Pattern polynomials that describe the
    //! gain and phase patterns
    six::sicd::GainAndPhasePolys element;

    //! Parameters that identify 2-D sampled Gain &
    //! Phase patterns at single frequency value.
    std::vector<GainPhaseArray> gainPhaseArray;
};

/*
 *  \struct Antenna
 *  \brief (Optional) Antenna parameters
 *
 *  Parameters that describe the transmit and
 *  receive antennas used to collect the signal array.
 *  See section 8.2
 */
struct Antenna
{
    //! Default constructor
    Antenna();

    //! Equality operators
    bool operator==(const Antenna& other) const
    {
        return antCoordFrame == other.antCoordFrame &&
                antPhaseCenter == other.antPhaseCenter &&
                antPattern == other.antPattern;
    }
    bool operator!=(const Antenna& other) const
    {
        return !((*this) == other);
    }

    //! Unit vectors that describe the orientation of an
    //! Antenna Coordinate Frame (ACF) as function of
    //! time.
    std::vector<AntCoordFrame> antCoordFrame;

    //! Parameters that describe each Antenna Phase
    //! Center (APC). Parameter set repeated for each
    //! APC
    std::vector<AntPhaseCenter> antPhaseCenter;

    //! Parameter set that defines each Antenna Pattern
    //! as function time. Parameters set repeated for
    //! each Antenna Pattern
    std::vector<AntPattern> antPattern;
};

//! Ostream operators
std::ostream& operator<< (std::ostream& os, const AntCoordFrame& a);
std::ostream& operator<< (std::ostream& os, const AntPhaseCenter& a);
std::ostream& operator<< (std::ostream& os, const AntPattern::GainPhaseArray& g);
std::ostream& operator<< (std::ostream& os, const AntPattern& a);
std::ostream& operator<< (std::ostream& os, const Antenna& a);
}

#endif

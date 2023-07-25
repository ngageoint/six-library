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
#ifndef SIX_cphd_Channel_h_INCLUDED_
#define SIX_cphd_Channel_h_INCLUDED_

#include <string>
#include <vector>
#include <std/optional>

#include <cphd/Types.h>
#include <cphd/Enums.h>
#include <cphd/SceneCoordinates.h>

namespace cphd
{
/*
*  \struct PolRefType
*
*  \brief Polarization parameters for the pulse for the reference signal vector.
* (New in CPHD 1.1.0)
*/
struct PolRef final
{
    bool operator==(const PolRef& other) const
    {
        return (ampH == other.ampH)
            && (ampV == other.ampV)
            && (phaseV == other.phaseV);
    }
    bool operator!=(const PolRef& other) const
    {
        return !(*this == other);
    }

    //! E-field relative amplitude in H direction
    ZeroToOne ampH = 0.0;

    //! E-field relative amplitude in V direction
    ZeroToOne ampV = 0.0;

    //! Relative phase of the V E-field relative to the H E-field
    NegHalfToHalf phaseV = 0.0;
};

/*
 *  \struct Polarization
 *
 *  \brief Polarization of the signals that formed the signal array.
 */
struct Polarization final
{
    //! Equality operator
    bool operator==(const Polarization& other) const
    {
        if ((txPol != other.txPol) || (rcvPol != other.rcvPol))
        {
            return false;
        }

        // The are optional: new in CPHD 1.1.0
        if ((txPolRef != other.txPolRef) || (rcvPolRef != other.rcvPolRef))
        {
            return false;
        }

        return true;
    }
    bool operator!=(const Polarization& other) const
    {
        return !((*this) == other);
    }

    //! Transmitted signal polarization for the channel
    PolarizationType txPol;

    //! Receive polarization for the channel.
    PolarizationType rcvPol;

    //! Transmit polarization parameters for the transmit pulse for the reference signal vector  (v_CH_REF). See Section 7.2.5.
    std::optional<PolRef> txPolRef; // new in CPHD 1.1.0

    //! Receive polarization parameters for the received signals for the reference signal vector (v_CH_REF). See Section 7.2.5.
    std::optional<PolRef> rcvPolRef; // new in CPHD 1.1.0
};

/*
 *  \struct TOAExtended
 *
 *  \brief (Optional) TOA extended swath information
 */
struct TOAExtended
{
    /*
     *  \struct LFMEclipse
     *
     *  \brief LFMEclipse parameters
     *
     *  (Optional) Parameters that describe the FX domain signal
     *  content for partially eclipsed echoes when the
     *  collection is performed with a Linear FM
     *  waveform.
     */
    struct LFMEclipse
    {
        //! Constructor
        LFMEclipse();

        //! Equality operator
        bool operator==(const LFMEclipse& other) const
        {
            return fxEarlyLow == other.fxEarlyLow &&
                    fxEarlyHigh == other.fxEarlyHigh &&
                    fxLateLow == other.fxLateLow &&
                    fxLateHigh == other.fxLateHigh;
        }
        bool operator!=(const LFMEclipse& other) const
        {
            return !((*this) == other);
        }

        //! FX domain minimum frequency value for an echo
        //! delta TOA = delta TOAE1 < delta TOA1
        double fxEarlyLow;

        //! FX domain maximum frequency value for an echo
        //! delta TOA = delta TOAE1 < delta TOA1
        double fxEarlyHigh;

        //! FX domain minimum frequency value for an echo
        //!  delta TOA = delta TOAE2 > delta TOA2
        double fxLateLow;

        //! FX domain maximum frequency value for an echo
        //! delta TOA = delta TOAE2 > delta TOA2
        double fxLateHigh;
    };

    //! Constructor
    TOAExtended();

    //! Equality operator
    bool operator==(const TOAExtended& other) const
    {
        return toaExtSaved == other.toaExtSaved &&
                lfmEclipse == other.lfmEclipse;
    }
    bool operator!=(const TOAExtended& other) const
    {
        return !((*this) == other);
    }

    //! TOA extended swath saved that includes both
    //! full and partially eclipsed echoes.
    double toaExtSaved;

    //! Parameters that describe the FX domain signal
    //! content
    mem::ScopedCopyablePtr<LFMEclipse> lfmEclipse;
};

/*
 *  \struct DwellTimes
 *
 *  \brief COD Time & Dwell Time polynomials
 *
 *  See section 6.3 and section 7.2.6
 */
struct DwellTimes
{
    //! COD Time & Dwell Time polynomials over the
    //! image area
    DwellTimes();

    // Equality operator
    bool operator==(const DwellTimes& other) const
    {
        return codId == other.codId &&
                dwellId == other.dwellId;
    }
    bool operator!=(const DwellTimes& other) const
    {
        return !((*this) == other);
    }

    //! Identifier of the center of Dwell Time polynomial
    //! that maps reference surface position to COD time
    std::string codId;

    //! Identifier of the dwell Time polynomial that
    //! maps reference surface position to dwell time
    std::string dwellId;
};

/*
 *  \struct TgtRefLevel
 *
 *  (Optional) Signal level for an ideal point scatter
 *  located at the SRP for reference signal vector (v_CH_REF)
 *  See section 4.6 and section 7.2.9
 */
struct TgtRefLevel
{
    // Constructor
    TgtRefLevel();

    // Equality operators
    bool operator==(const TgtRefLevel& other) const
    {
        return ptRef == other.ptRef;
    }
    bool operator!=(const TgtRefLevel& other) const
    {
        return !((*this) == other);
    }

    //! Target power level for a 1.0 square meter ideal
    //! point scatterer located at the SRP.
    double ptRef;
};

/*
 *  \struct Point
 *
 *  \brief Points that describe the noise profile
 */
struct Point
{
    //! Constructor
    Point();

    //! Equality operators
    bool operator==(const Point& other) const
    {
        return fx == other.fx &&
                pn == other.pn;
    }
    bool operator!=(const Point& other) const
    {
        return !((*this) == other);
    }

    //! Frequency value of this noise profile point
    double fx;

    //! Power level of this noise profile point
    double pn;
};

/*
 *  \struct FxNoiseProfile
 *
 *  \brief (Optional) Power level for thermal noise (PN) vs FX freq values
 */
struct FxNoiseProfile
{
    //! Equality operators
    bool operator==(const FxNoiseProfile& other) const
    {
        return point == other.point;
    }
    bool operator!=(const FxNoiseProfile& other) const
    {
        return !((*this) == other);
    }

    //! Points that describe the noise profile
    std::vector<Point> point;
};

/*
 *  \struct NoiseLevel
 *
 *  \brief (Optional) Thermal noise level for the reference signal vector
 */
struct NoiseLevel
{
    //! Thermal noise level for the reference signal
    //! vector (v_CH_REF).
    NoiseLevel();

    //! Equality operators
    bool operator==(const NoiseLevel& other) const
    {
        return pnRef == other.pnRef &&
                bnRef == other.bnRef &&
                fxNoiseProfile == other.fxNoiseProfile;
    }
    bool operator!=(const NoiseLevel& other) const
    {
        return !((*this) == other);
    }

    //! Noise power level for thermal noise.
    double pnRef;

    //! Noise Equivalent BW for noise signal.
    double bnRef;

    //! (Optional) FX Domain Noise Level Profile. Power level for
    //! thermal noise (PN) vs. FX frequency values
    mem::ScopedCopyablePtr<FxNoiseProfile> fxNoiseProfile;
};

/*
 *  \sturct ChannelParameter
 *
 *  \brief Channel parameters
 *
 *  Parameter Set that describes a CPHD data
 *  channel. Channels referenced by their unique
 *  Channel ID (Ch_ID). See Section 7.2.
*/
struct ChannelParameter
{
    /*
     *  \struct TxRcv
     *
     *  \brief Transmit Receive parameters
     *
     *  (Optional) Parameters to identify the Transmit and Receive
     *  parameter sets used to collect the signal array
     */
    // Hiding TxRcv struct here because of name conflict
    struct TxRcv
    {
        //! Equality operators
        bool operator==(const TxRcv& other) const
        {
            return txWFId == other.txWFId &&
                    rcvId == other.rcvId;
        }
        bool operator!=(const TxRcv& other) const
        {
            return !((*this) == other);
        }

        //! Identifier of the Transmit Waveform parameter
        //! set(s) that were used
        std::vector<std::string> txWFId;

        //! Identifier of the Receive Parameter set(s) that
        //! were used.
        std::vector<std::string> rcvId;
    };

    /*
     *  \struct Antenna
     *
     *  \brief Channel antenna parameters
     *
     *  (Optional) Antenna Phase Center and Antenna Pattern
     *  identifiers for the antenna(s) used to collect and
     *  form the signal array data.
     */
    // Hiding Antenna struct here because of naming clash
    struct Antenna
    {
        // Constructor
        Antenna();

        // Equality operators
        bool operator==(const Antenna& other) const
        {
            return txAPCId == other.txAPCId &&
                    txAPATId == other.txAPATId &&
                    rcvAPCId == other.rcvAPCId &&
                    rcvAPATId == other.rcvAPATId;
        }
        bool operator!=(const Antenna& other) const
        {
            return !((*this) == other);
        }

        //! Identifier of Transmit APC to be used to compute
        //! the transmit antenna pattern as a function of time
        //! for the channel (APC_ID).
        std::string txAPCId;

        //! Identifier of Transmit Antenna pattern used to
        //! form the channel signal array (APAT_ID)
        std::string txAPATId;

        //! Identifier of Receive APC to be used to compute
        //! the receive antenna pattern as a function of time
        //! for the channel (APC_ID)
        std::string rcvAPCId;

        //! Identifier of Receive Antenna pattern used to
        //! form the channel (APAT_ID).
        std::string rcvAPATId;
    };

    //! Default constructor
    ChannelParameter();

    //! Equality operators
    bool operator==(const ChannelParameter& other) const
    {
        return identifier == other.identifier &&
                refVectorIndex == other.refVectorIndex &&
                fxFixed == other.fxFixed && toaFixed == other.toaFixed &&
                srpFixed == other.srpFixed && signalNormal == other.signalNormal &&
                polarization == other.polarization && fxC == other.fxC &&
                fxBW == other.fxBW && fxBWNoise == other.fxBWNoise &&
                toaSaved == other.toaSaved && toaExtended == other.toaExtended &&
                dwellTimes == other.dwellTimes && imageArea == other.imageArea &&
                antenna == other.antenna && txRcv == other.txRcv && tgtRefLevel == other.tgtRefLevel &&
                noiseLevel == other.noiseLevel;
    }
    bool operator!=(const ChannelParameter& other) const
    {
        return !((*this) == other);
    }

    //! String that uniquely identifies this CPHD data
    //! channel (Ch_ID).
    std::string identifier;

    //! Index of the reference vector (v_CH_REF) for
    //! the channel.
    size_t refVectorIndex;

    //! Flag to indicate when a constant FX band is
    //! saved for all signal vectors of the channel.
    six::BooleanType fxFixed;

    //! Flag to indicate when a constant TOA swath is
    //! saved for all signal vectors of the channel.
    six::BooleanType toaFixed;

    //! Flag to indicate when a constant SRP position is
    //! used all signal vectors of the channel.
    six::BooleanType srpFixed;

    //! (Optional) Flag to indicate when all signal array
    //! vectors are normal
    six::BooleanType signalNormal;

    //! Polarization(s) of the signals that formed the
    //! signal array
    Polarization polarization;

    //! FX center frequency value for saved bandwidth
    //! for the channel. Computed from all vectors of
    //! the signal array
    double fxC;

    //! FX band spanned for the saved bandwidth for the
    //! channel. Computed from all vectors of the signal
    //! array
    double fxBW;

    //! (Optional) FX signal bandwidth saved that includes noise
    //! signal below or above the retained echo signal
    //! bandwidth. Included when fx_N1 < fx_1 or fx_2
    //! < fx_N2 for any vector
    double fxBWNoise; // Optional

    //! TOA swath saved for the full resolution echoes
    //! for the channel. Computed from all vectors in the
    //! signal array.
    double toaSaved;

    //! COD Time & Dwell Time polynomials over the
    //! image area
    DwellTimes dwellTimes;

    //! (Optional) Image Area for the CPHD channel defined
    //! by a rectangle aligned with (IAX, IAY). May be
    //! reduced by the optional polygon
    AreaType imageArea;

    //! (Optional) TOA extended swath information.
    mem::ScopedCopyablePtr<TOAExtended> toaExtended;

    //! (Optional) Antenna Phase Center and Antenna Pattern
    //! identifiers for the antenna(s) used to collect and
    //! form the signal array data.
    mem::ScopedCopyablePtr<Antenna> antenna;

    //! (Optional) Parameters to identify the Transmit and Receive
    //! parameter sets used to collect the signal array
    mem::ScopedCopyablePtr<TxRcv> txRcv;

    //! (Optional) Signal level for an ideal point scatterer located at
    //! the SRP for reference signal vector (v_CH_REF)
    mem::ScopedCopyablePtr<TgtRefLevel> tgtRefLevel;

    //! (Optional) Thermal noise level for the reference signal
    //! vector (v_CH_REF)
    mem::ScopedCopyablePtr<NoiseLevel> noiseLevel;
};

/*
 *  \struct Channel
 *
 *  \brief Channel parameters
 *
 *  Parameters that describe the data channels
 *  contained in the product.
 *  See section 7.
 */
struct Channel
{
    //! Constructor
    Channel();

    //! Equality operators
    bool operator==(const Channel& other) const
    {
        return refChId == other.refChId &&
               fxFixedCphd == other.fxFixedCphd &&
               toaFixedCphd == other.toaFixedCphd &&
               srpFixedCphd == other.srpFixedCphd &&
               parameters == other.parameters &&
               addedParameters == other.addedParameters;
    }
    bool operator!=(const Channel& other) const
    {
        return !((*this) == other);
    }

    //! Channel ID (Ch_ID) for the Reference Channel
    //! in the product
    std::string refChId;

    //! Flag to indicate when a constant FX band is
    //! saved for all signal vectors of all channels
    six::BooleanType fxFixedCphd;

    //! Flag to indicate when a constant TOA swath is
    //! saved for all signal vectors of all channels
    six::BooleanType toaFixedCphd;

    //! Flag to indicate when a constant SRP position is
    //! used all signal vectors of all channels
    six::BooleanType srpFixedCphd;

    //! Parameter Set that describes a CPHD data
    //! channel. Channels referenced by their unique
    //! Channel ID (Ch_ID).
    std::vector<ChannelParameter> parameters;

    //! (Optional) Block for including additional parameters that
    //! describe the channels and/or signal arrays.
    six::ParameterCollection addedParameters;
};

//! Ostream operators
std::ostream& operator<< (std::ostream& os, const PolRef& p);
std::ostream& operator<< (std::ostream& os, const Polarization& p);
std::ostream& operator<< (std::ostream& os, const TOAExtended& t);
std::ostream& operator<< (std::ostream& os, const DwellTimes& d);
std::ostream& operator<< (std::ostream& os, const TgtRefLevel& t);
std::ostream& operator<< (std::ostream& os, const Point& p);
std::ostream& operator<< (std::ostream& os, const FxNoiseProfile& f);
std::ostream& operator<< (std::ostream& os, const NoiseLevel& n);
std::ostream& operator<< (std::ostream& os, const ChannelParameter& c);
std::ostream& operator<< (std::ostream& os, const Channel& c);
}

#endif // SIX_cphd_Channel_h_INCLUDED_

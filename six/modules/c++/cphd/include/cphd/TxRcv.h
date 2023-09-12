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

#ifndef __CPHD_TXRCV_H__
#define __CPHD_TXRCV_H__

#include <ostream>
#include <vector>
#include <stddef.h>

#include <cphd/Enums.h>
#include <cphd/Types.h>


namespace cphd
{

/*!
 *  \struct ParameterType
 *
 *  \brief Base parameter class for both TxWF and Rcv type parameters
 */
struct ParameterType
{
    //! Constructor
    ParameterType();

    //! Equality operators
    bool operator==(const ParameterType& other) const
    {
        return identifier == other.identifier &&
                freqCenter == other.freqCenter &&
                lfmRate == other.lfmRate &&
                polarization == other.polarization;
    }

    bool operator!=(const ParameterType& other) const
    {
        return !((*this) == other);
    }

    //! String that uniquely identifies this Transmit Waveform
    std::string identifier;

    //! Center frequency of the transmitted waveform
    double freqCenter;

    //! (Optional) Chirp rate of transmitted pulse if LFM
    // Allowed lfmRate != 0
    double lfmRate;

    //! Transmit polarization
    PolarizationType polarization;
};

/*!
 *  \struct TxWFParameters
 *
 *  \brief Parameters that describe a Transmit Waveform
 */
struct TxWFParameters : public ParameterType
{
    //! Constructor
    TxWFParameters();

    //! Equality operators
    bool operator==(const TxWFParameters& other) const
    {
        return pulseLength == other.pulseLength &&
                rfBandwidth == other.rfBandwidth &&
                power == other.power;
    }
    bool operator!=(const TxWFParameters& other) const
    {
        return !((*this) == other);
    }

    //! Length of transmitted pulse
    double pulseLength;

    //! Bandwidth if transmitted pulse
    double rfBandwidth;

    //! Peak transmitted power at the interface to the
    //! antenna
    double power;
};

/*
 *  \struct RcvParameters
 *
 *  \brief Parameters that describe a Receive configuration
 */
struct RcvParameters : public ParameterType
{
    //! Constructor
    RcvParameters();

    //! Equality operators
    bool operator==(const RcvParameters& other) const
    {
        return windowLength == other.windowLength &&
                sampleRate == other.sampleRate &&
                ifFilterBW == other.ifFilterBW &&
                pathGain == other.pathGain;
    }
    bool operator!=(const RcvParameters& other) const
    {
        return !((*this) == other);
    }

    //! Length of the receive window
    double windowLength;

    //! Rate at which the signal in the receive window
    //! is sampled
    double sampleRate;

    //! Bandwidth of the anti-aliasing filter prior to
    //! sampling
    double ifFilterBW;

    //! Receiver gain from the antenna interface to the
    //! ADC.
    double pathGain;
};

/*!
 *  \struct TxRcv
 *
 *  (Optional) Parameters that describe the transmitted
 *  waveform(s) and receiver configurations used in
 *  the collection
 */
struct TxRcv
{
    //! Constructor
    TxRcv();

    //! Equality operators
    bool operator==(const TxRcv& other) const
    {
        return txWFParameters == other.txWFParameters &&
                rcvParameters == other.rcvParameters;
    }
    bool operator!=(const TxRcv& other) const
    {
        return !((*this) == other);
    }

    //! Parameters that describe a Transmit Waveform
    std::vector<TxWFParameters> txWFParameters;

    //! Parameters that describe a Receive configuration
    std::vector<RcvParameters> rcvParameters;
};

//! Ostream operators
std::ostream& operator<< (std::ostream& os, const ParameterType& p);
std::ostream& operator<< (std::ostream& os, const TxWFParameters& t);
std::ostream& operator<< (std::ostream& os, const RcvParameters& r);
std::ostream& operator<< (std::ostream& os, const TxRcv& t);
}

#endif

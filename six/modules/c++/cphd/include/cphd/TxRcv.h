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

struct ParameterType
{
    ParameterType();

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

    std::string identifier;
    double freqCenter;
    double lfmRate; // Allowed lfmRate != 0
    PolarizationType polarization;

};

struct TxWFParameters : public ParameterType
{
    TxWFParameters();

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

    double pulseLength;
    double rfBandwidth;
    double power;
};

struct RcvParameters : public ParameterType
{
    RcvParameters();

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

    double windowLength;
    double sampleRate;
    double ifFilterBW;
    double pathGain;
};

struct TxRcv
{
    TxRcv();

    bool operator==(const TxRcv& other) const
    {
        return numTxWFs == other.numTxWFs &&
                numRcvs == other.numRcvs &&
                txWFParameters == other.txWFParameters &&
                rcvParameters == other.rcvParameters;
    }

    bool operator!=(const TxRcv& other) const
    {
        return !((*this) == other);
    }

    size_t numTxWFs;
    size_t numRcvs;
    std::vector<TxWFParameters> txWFParameters;
    std::vector<RcvParameters> rcvParameters;
};

std::ostream& operator<< (std::ostream& os, const ParameterType& p);
std::ostream& operator<< (std::ostream& os, const TxWFParameters& t);
std::ostream& operator<< (std::ostream& os, const RcvParameters& r);
std::ostream& operator<< (std::ostream& os, const TxRcv& t);
}

#endif

/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2018, MDA Information Systems LLC
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

#ifndef __CPHD_CHANNEL_H__
#define __CPHD_CHANNEL_H__

#include <string>
#include <vector>
#include <cphd/Types.h>
#include <cphd/Enums.h>
#include <cphd/SceneCoordinates.h>


namespace cphd
{

/*
 * Polarization of the signals that formed the signal array.
 */
struct Polarization
{
  bool operator==(const Polarization& other) const
  {
    return txPol == other.txPol &&
           rcvPol == other.rcvPol;
  }

  bool operator!=(const Polarization& other) const
  {
    return !((*this) == other);
  }

  PolarizationType txPol;
  PolarizationType rcvPol;
};

/*
 * (Optional) TOA extended swath information
 */
struct TOAExtended
{
    struct LFMEclipse
    {
        LFMEclipse();

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

        double fxEarlyLow;
        double fxEarlyHigh;
        double fxLateLow;
        double fxLateHigh;
    };

    TOAExtended();

    bool operator==(const TOAExtended& other) const
    {
        return toaExtSaved == other.toaExtSaved &&
                lfmEclipse == other.lfmEclipse;
    }

    bool operator!=(const TOAExtended& other) const
    {
        return !((*this) == other);
    }

    double toaExtSaved;
    mem::ScopedCopyablePtr<LFMEclipse> lfmEclipse;
};

/*
 * COD Time & Dwell Time polynomials over the image area
 * See section 6.3 and section 7.2.6
 */
struct DwellTimes
{
    DwellTimes();

    bool operator==(const DwellTimes& other) const
    {
        return codId == other.codId &&
                dwellId == other.dwellId;
    }

    bool operator!=(const DwellTimes& other) const
    {
        return !((*this) == other);
    }

    std::string codId;
    std::string dwellId;
};

/*
 * (Optional) Signal level for an ideal point scatter
 * located at the SRP for reference signal vector (v_CH_REF)
 * See section 4.6 and section 7.2.9
 */
struct TgtRefLevel
{
    TgtRefLevel();

    bool operator==(const TgtRefLevel& other) const
    {
        return ptRef == other.ptRef;
    }

    bool operator!=(const TgtRefLevel& other) const
    {
        return !((*this) == other);
    }

    double ptRef;
};

struct Point
{
    Point();

    bool operator==(const Point& other) const
    {
        return fx == other.fx &&
                pn == other.pn;
    }

    bool operator!=(const Point& other) const
    {
        return !((*this) == other);
    }

    double fx;
    double pn;
};

/*
 * (Optional) Power level for thermal noise (PN) vs FX freq values
 */
struct FxNoiseProfile 
{
    bool operator==(const FxNoiseProfile& other) const
    {
        return point == other.point;
    }

    bool operator!=(const FxNoiseProfile& other) const
    {
        return !((*this) == other);
    }

    std::vector<Point> point;
};

/*
 * (Optional) Thermal noise level for the reference signal vector
 */
struct NoiseLevel
{
    NoiseLevel();

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

    double pnRef;
    double bnRef;
    mem::ScopedCopyablePtr<FxNoiseProfile> fxNoiseProfile;
};

/* Parameter Set that describes a CPHD data
channel. Channels referenced by their unique
Channel ID (Ch_ID). See Section 7.2. */
struct ChannelParameter
{
    // Hiding TxRcv struct here because of name conflict //
    struct TxRcv
    {
        bool operator==(const TxRcv& other) const
        {
            return txWFId == other.txWFId &&
                    rcvId == other.rcvId;
        }

        bool operator!=(const TxRcv& other) const
        {
            return !((*this) == other);
        }

        std::vector<std::string> txWFId;
        std::vector<std::string> rcvId;
    };

    // Hiding Antenna struct here because of naming clash //
    struct Antenna
    {
        Antenna();

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

        std::string txAPCId;
        std::string txAPATId;
        std::string rcvAPCId;
        std::string rcvAPATId;
    };

    ChannelParameter();

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

    std::string identifier;
    size_t refVectorIndex;
    six::BooleanType fxFixed;
    six::BooleanType toaFixed;
    six::BooleanType srpFixed;
    six::BooleanType signalNormal;
    Polarization polarization;
    double fxC;
    double fxBW;
    double fxBWNoise;
    double toaSaved;
    DwellTimes dwellTimes;
    AreaType imageArea;
    mem::ScopedCopyablePtr<TOAExtended> toaExtended;
    mem::ScopedCopyablePtr<Antenna> antenna;
    mem::ScopedCopyablePtr<TxRcv> txRcv;
    mem::ScopedCopyablePtr<TgtRefLevel> tgtRefLevel;
    mem::ScopedCopyablePtr<NoiseLevel> noiseLevel;
};

/*
 * Parameters that describe the data channels 
 * contained in the product.
 * See section 7.
 */
struct Channel
{
    Channel();

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

    std::string refChId;
    six::BooleanType fxFixedCphd;
    six::BooleanType toaFixedCphd;
    six::BooleanType srpFixedCphd;
    std::vector<ChannelParameter> parameters;
    six::ParameterCollection addedParameters;
};

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

#endif

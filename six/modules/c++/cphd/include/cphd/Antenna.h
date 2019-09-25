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

#include <cphd/Enums.h>
#include <cphd/Types.h>
#include <six/sicd/Antenna.h>

namespace cphd
{

/*!
 * Unit vectors that describe the orientation of an 
 * Antenna Coordinate Frame as a function of time.
 */
struct AntCoordFrame
{
    AntCoordFrame();

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

    std::string identifier;
    PolyXYZ xAxisPoly;
    PolyXYZ yAxisPoly;
};

/*
 * Parameters that describe each Antenna Phase Center.
 */
struct AntPhaseCenter
{
    AntPhaseCenter();

    bool operator==(const AntPhaseCenter& other) const
    {
        return identifier == other.identifier &&
                acfId == other.acfId && apcXYZ == other.apcXYZ;
    }

    bool operator!=(const AntPhaseCenter& other) const
    {
        return !((*this) == other);
    }

    std::string identifier;
    std::string acfId;
    Vector3 apcXYZ;
};

/*
 * Parameter set that defines each Antenna Pattern
 * as a function of time.
 */
struct AntPattern
{
    /*
     * Parameters that identify 2-D sampled Grain & Phase patterns at
     * a single frequency value.
     * See table 11-7
     */
    struct GainPhaseArray
    {
        bool operator==(const GainPhaseArray& other) const
        {
            return freq == other.freq && arrayId == other.arrayId &&
                    elementId == other.elementId;
        }

        bool operator!=(const GainPhaseArray& other) const
        {
            return !((*this) == other);
        }

        double freq;
        std::string arrayId;
        std::string elementId;
    };

    AntPattern();

    bool operator==(const AntPattern& other) const
    {
        return freqZero == other.freqZero && gainZero == other.gainZero &&
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

    // Unique ID value for each set of AntPattern parameter set
    std::string identifier;
    double freqZero;
    double gainZero;
    six::BooleanType ebFreqShift;
    six::BooleanType mlFreqDilation;
    Poly1D gainBSPoly;
    six::sicd::ElectricalBoresight eb;
    six::sicd::GainAndPhasePolys array;
    six::sicd::GainAndPhasePolys element;
    std::vector<GainPhaseArray> gainPhaseArray;
};

/*!
 * (Optional) Parameters that describe the transmit and
 * receive antennas used to collect the signal array.
 * See section 8.2
 */
struct Antenna
{
    Antenna();

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

    std::vector<AntCoordFrame> antCoordFrame;
    std::vector<AntPhaseCenter> antPhaseCenter;
    std::vector<AntPattern> antPattern;
};

std::ostream& operator<< (std::ostream& os, const AntCoordFrame& a);
std::ostream& operator<< (std::ostream& os, const AntPhaseCenter& a);
std::ostream& operator<< (std::ostream& os, const AntPattern::GainPhaseArray& g);
std::ostream& operator<< (std::ostream& os, const AntPattern& a);
std::ostream& operator<< (std::ostream& os, const Antenna& a);
}

#endif

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


namespace cphd
{

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

struct AntPattern
{
    struct EB
    {
        bool operator==(const EB& other) const
        {
            return dcXPoly == other.dcXPoly && dcYPoly == other.dcYPoly;
        }

        bool operator!=(const EB& other) const
        {
            return !((*this) == other);
        }

        Poly1D dcXPoly;
        Poly1D dcYPoly;
    };

    struct ArrayElement
    {
        bool operator==(const ArrayElement& other) const
        {
            return gainPoly == other.gainPoly && phasePoly == other.phasePoly;
        }

        bool operator!=(const ArrayElement& other) const
        {
            return !((*this) == other);
        }

        Poly2D gainPoly;
        Poly2D phasePoly;
    };


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
    EB eb;
    ArrayElement array;
    ArrayElement element;
    std::vector<GainPhaseArray> gainPhaseArray;

};


struct Antenna
{
    Antenna();

    bool operator==(const Antenna& other) const
    {
        return numACFs == other.numACFs &&
                numAPCs == other.numAPCs &&
                numAntPats == other.numAntPats;
    }

    bool operator!=(const Antenna& other) const
    {
        return !((*this) == other);
    }

    double numACFs;
    double numAPCs;
    double numAntPats;
    std::vector<AntCoordFrame> antCoordFrame;
    std::vector<AntPhaseCenter> antPhaseCenter;
    std::vector<AntPattern> antPattern;

};
std::ostream& operator<< (std::ostream& os, const AntCoordFrame& a);
std::ostream& operator<< (std::ostream& os, const AntPhaseCenter& a);
std::ostream& operator<< (std::ostream& os, const AntPattern::EB& e);
std::ostream& operator<< (std::ostream& os, const AntPattern::ArrayElement& a);
std::ostream& operator<< (std::ostream& os, const AntPattern::GainPhaseArray& g);
std::ostream& operator<< (std::ostream& os, const AntPattern& a);
std::ostream& operator<< (std::ostream& os, const Antenna& a);
}

#endif

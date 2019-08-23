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


#ifndef __CPHD_DATA_H__
#define __CPHD_DATA_H__

#include <ostream>
#include <vector>
#include <stddef.h>

#include <cphd/Enums.h>

namespace cphd
{

struct Data
{
    // There's another SupportArray type in the Metadata,
    // so hiding this inside Data
    struct SupportArray
    {
        SupportArray();

        bool operator==(const SupportArray& other) const
        {
            return identifier == other.identifier &&
                   numRows == other.numRows &&
                   numCols == other.numCols &&
                   bytesPerElement == other.bytesPerElement &&
                   arrayByteOffset == other.arrayByteOffset;
        }

        bool operator!=(const SupportArray& other) const
        {
            return !((*this) == other);
        }

        std::string identifier;
        size_t numRows;
        size_t numCols;
        size_t bytesPerElement;
        size_t arrayByteOffset;
    };


    // There's another Channel type in the Metadata,
    // so hiding this inside Data
    struct Channel
    {
        Channel();

        bool operator==(const Channel& other) const
        {
            return identifier == other.identifier &&
                   numVectors == other.numVectors &&
                   numSamples == other.numSamples &&
                   signalArrayByteOffset == other.signalArrayByteOffset &&
                   pvpArrayByteOffset == other.pvpArrayByteOffset &&
                   compressedSignalSize == other.compressedSignalSize;
        }

        bool operator!=(const Channel& other) const
        {
            return !((*this) == other);
        }

        std::string identifier;
        size_t numVectors;
        size_t numSamples;
        size_t signalArrayByteOffset;
        size_t pvpArrayByteOffset;
        size_t compressedSignalSize; // Optional
    };

    Data();

    bool operator==(const Data& other) const
    {
        if (!(signalArrayFormat == other.signalArrayFormat &&
               numBytesPVP == other.numBytesPVP &&
               signalCompressionID == other.signalCompressionID))
        {
            return false;
        }

        if (channels.size() != other.channels.size())
        {
            return false;
        }
        for (size_t ii = 0; ii < channels.size(); ++ii)
        {
            if (channels[ii] != other.channels[ii])
            {
                return false;
            }
        }

        if (supportArrays.size() != other.supportArrays.size())
        {
            return false;
        }
        for (size_t ii = 0; ii < supportArrays.size(); ++ii)
        {
            if (supportArrays[ii] != other.supportArrays[ii])
            {
                return false;
            }
        }

        return true;
    }

    bool operator!=(const Data& other) const
    {
        return !((*this) == other);
    }

    SignalArrayFormat signalArrayFormat;
    size_t numBytesPVP;
    std::vector<Channel> channels;
    std::string signalCompressionID;
    std::vector<SupportArray> supportArrays;
};
std::ostream& operator<< (std::ostream& os, const Data::SupportArray& s);
std::ostream& operator<< (std::ostream& os, const Data::Channel& c);
std::ostream& operator<< (std::ostream& os, const Data& d);
}

#endif

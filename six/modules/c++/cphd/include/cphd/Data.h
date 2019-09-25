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

#ifndef __CPHD_DATA_H__
#define __CPHD_DATA_H__

#include <ostream>
#include <vector>
#include <stddef.h>

#include <cphd/Enums.h>

namespace cphd
{

/*
 * Parameter that describes binary data components
 * contained in the product.
 * See section 2.3 through 2.6
 */
struct Data
{
    // There's another SupportArray type in the Metadata,
    // so hiding this inside Data
    struct SupportArray
    {
        /*
         * (Optional) SUpport Array size parameters. 
         * Branch repeated for each binary support array
         * Support array referenced by its unique 
         * support array identifier SA_ID
         */
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


    /*
     * Paramters that define the Channel signal array and
     * PVP array size and location.
     * See section 2.4 and 2.5
     */
    // There's another Channel type in the Metadata,
    // so hiding this inside Data
    struct Channel
    {
        Channel();

        Channel(size_t vectors, size_t samples);

        Channel(size_t vectors, size_t samples,
                size_t signalByteOffset, size_t pvpByteOffset,
                size_t compressedSize);

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

        size_t getNumVectors() const
        {
            return numVectors;
        }
        size_t getNumSamples() const
        {
            return numSamples;
        }
        size_t getSignalArrayByteOffset() const
        {
            return signalArrayByteOffset;
        }
        size_t getCompressedSignalSize() const
        {
            return compressedSignalSize;
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
        return signalArrayFormat == other.signalArrayFormat &&
               numBytesPVP == other.numBytesPVP &&
               signalCompressionID == other.signalCompressionID &&
               channels == other.channels && supportArrays == other.supportArrays;
    }
    bool operator!=(const Data& other) const
    {
        return !((*this) == other);
    }

    size_t getNumVectors(size_t channel) const
    {
        if (channel < channels.size())
        {
            return channels[channel].getNumVectors();
        }
        throw except::Exception(Ctxt(
                "Invalid channel number provided"));
    }
    size_t getNumSamples(size_t channel) const
    {
        if (channel < channels.size())
        {
            return channels[channel].getNumSamples();
        }
        throw except::Exception(Ctxt(
                "Invalid channel number provided"));
    }
    size_t getCompressedSignalSize(size_t channel) const
    {
        if (channel < channels.size())
        {
            return channels[channel].getCompressedSignalSize();
        }
        throw except::Exception(Ctxt(
                "Invalid channel number provided"));
    }
    size_t getNumChannels() const
    {
        return channels.size();
    }
    size_t getNumBytesPVPSet() const
    {
        return numBytesPVP;
    }
    std::string getCompressionID() const
    {
        return signalCompressionID;
    }
    SignalArrayFormat getSignalFormat() const
    {
        return signalArrayFormat;
    }

    bool isCompressed() const;

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

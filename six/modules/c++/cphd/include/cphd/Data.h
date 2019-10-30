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
#include <map>
#include <unordered_map>
#include <assert.h>
#include <stddef.h>

#include <six/Init.h>
#include <sys/Conf.h>
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

    /*
     * Support Array size parameters.
     * Branch repeated for each binary support array
     * Support array referenced by its unique
     * support array identifier SA_ID
     */
    struct SupportArray
    {

        //! Constructor
        SupportArray();

        //! Custom constructor
        //! Initalizes a support array with necessary params
        SupportArray(const std::string& id, size_t rows, size_t cols,
                size_t numBytes, size_t offset);

        //! Equality Operators
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

        //! Get byte size of support array
        size_t getSize() const
        {
            return numRows * numCols * bytesPerElement;
        }

        //! Unique string that identifies this support array
        //! (SA_ID).
        std::string identifier;

        //! Number of rows in the array, NumRowsSA
        size_t numRows;

        //! Number of columns per row in the array,
        //! NumColsSA
        size_t numCols;

        //! Indicates the size in bytes of each data element in
        //! the support array.
        size_t bytesPerElement;

        //! Array offset from the start of the Support block
        //! (in bytes) to the start of the support array.
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
        //! Constructor
        Channel();

        //! Custom constructor
        Channel(size_t vectors, size_t samples);

        //! Custom constructor
        //!
        Channel(size_t vectors, size_t samples,
                size_t signalByteOffset, size_t pvpByteOffset,
                size_t compressedSize = six::Init::undefined<size_t>());

        //! Equality operators
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

        //! Getter functions
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

        //! String that uniquely identifies the CPHD channel
        std::string identifier;

        //! Number of vectors in the signal array
        size_t numVectors;

        //! Number of samples per vector in the signal array
        size_t numSamples;

        //! Signal Array offset from the start of the Signal
        //! block (in bytes) to the start of the Signal Array
        //! for the channel.
        size_t signalArrayByteOffset;

        //! PVP Array offset from the start of the PVP block
        //! (in bytes) to the start of the PVP Array for the
        //! channel.
        size_t pvpArrayByteOffset;

        //! (Optional) Size (in bytes) of the compressed signal array
        //! byte sequence for the data channel.
        size_t compressedSignalSize; // Optional
    };

    //! Constructor
    Data();

    //! Equality operator
    bool operator==(const Data& other) const
    {
        return signalArrayFormat == other.signalArrayFormat &&
               numBytesPVP == other.numBytesPVP &&
               signalCompressionID == other.signalCompressionID &&
               channels == other.channels &&
               supportArrayMap == other.supportArrayMap &&
               mOffsetMap == other.mOffsetMap;
    }
    bool operator!=(const Data& other) const
    {
        return !((*this) == other);
    }

    //! Getter functions
    size_t getNumVectors(size_t channel) const
    {
        if (channel < channels.size())
        {
            return channels[channel].getNumVectors();
        }
        std::ostringstream ostr;
        ostr << "Channel provided is " << channel << "\n"
                << "while only " << channels.size()
                << " channels exist \n";
        throw except::Exception(ostr.str());
    }
    size_t getNumSamples(size_t channel) const
    {
        if (channel < channels.size())
        {
            return channels[channel].getNumSamples();
        }
        std::ostringstream ostr;
        ostr << "Channel provided is " << channel << "\n"
                << "while only " << channels.size()
                << " channels exist \n";
        throw except::Exception(ostr.str());
    }
    size_t getCompressedSignalSize(size_t channel) const
    {
        if (channel < channels.size())
        {
            return channels[channel].getCompressedSignalSize();
        }
        std::ostringstream ostr;
        ostr << "Channel provided is " << channel << "\n"
                << "while only " << channels.size()
                << " channels exist \n";
        throw except::Exception(ostr.str());
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
    size_t getNumSupportArrays() const
    {
        return supportArrayMap.size();
    }
    //! Get specific support array by identifier
    SupportArray getSupportArrayById(const std::string& id) const;

    //! Get size of support array element
    size_t getElementSize(const std::string& id) const
    {
        return getSupportArrayById(id).bytesPerElement;
    }
    size_t getAllSupportSize() const;

    //! Create and add new support array
    //! Validates, and add new support array to supportArrayMap
    void setSupportArray(const std::string& id, size_t numRows,
                         size_t numCols, size_t numBytes,
                         sys::Off_T offset);

    //! Checks if wideband data is compressed
    bool isCompressed() const;

public:
    //! Signal Array sample binary format of the CPHD
    //! signal arrays in standard (i.e. uncompressed)
    //! format.
    SignalArrayFormat signalArrayFormat;

    //! Number of bytes per set of Per Vector
    //! Parameters. One set of PVPs for each CPHD
    //! signal vector.
    size_t numBytesPVP;

    //! Parameters that define the Channel signal array
    //! and PVP array size and location
    std::vector<Channel> channels;

    //! (Optional) Parameter that indicates the signal arrays are in
    //! compressed format. Value identifies the method
    //! of decompression
    std::string signalCompressionID;

    // Both of these maps get populated and/or edited together
    // Made for O(logn) lookup
    //! (Optional) Map of unique support array id to support array offset
    // std::unordered_map<std::string,sys::Off_T> supportOffsetMap;
    // (Optional) Ordered map to store support arrays with offset key
    // std::map<sys::Off_T,SupportArray, CmpByOffset> supportArrayMap;

    // (Optional) Unordered map key: unique identifier string,
    // value: supportArray object
    std::unordered_map<std::string, SupportArray> supportArrayMap;

private:
    // Custom comparator
    struct CmpByOffset
    {
        // Overload operator ()
        // Returns true if offset lhs is less than other offset
        bool operator()(const sys::Off_T& lhs, const sys::Off_T& rhs) const
        {
            return lhs < rhs;
        }
    };

    // Book keeping map for efficient validation
    // Support Array Map with:
    // key: offset, value: array size
    std::map<sys::Off_T, size_t, CmpByOffset> mOffsetMap;
};

//! Ostream operators
std::ostream& operator<< (std::ostream& os, const Data::SupportArray& s);
std::ostream& operator<< (std::ostream& os, const Data::Channel& c);
std::ostream& operator<< (std::ostream& os, const Data& d);
}

#endif

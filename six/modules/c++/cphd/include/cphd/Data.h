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
#ifndef __CPHD_DATA_H__
#define __CPHD_DATA_H__

#include <ostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <assert.h>
#include <stddef.h>

#include <scene/sys_Conf.h>
#include <six/Init.h>
#include <cphd/Enums.h>
#include <cphd/Utilities.h>
#include <cphd/Exports.h>

namespace cphd
{

/*
 *  \struct Data
 *
 *  \brief Parameter that describes binary data components
 *
 *  See section 2.3 through 2.6
 */
struct SIX_CPHD_API Data
{
    /*
     *  \struct SupportArray
     *
     *  \brief Support Array size parameters.
     *
     *  Branch repeated for each binary support array
     *  Support array referenced by its unique
     *  support array identifier SA_ID
     */
    // There's another SupportArray type in the Metadata,
    // so hiding this inside Data
    struct SIX_CPHD_API SupportArray
    {

        /*
         *  \func SupportArray
         *
         *  \brief Default constructor
         */
        SupportArray();

        /*
         *  \func SupportArray
         *
         *  \brief Custom constructor initializes all parameters
         */
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
        size_t size() const
        {
            return numRows * numCols;
        }
        size_t size_bytes() const // c.f., std::span::size_bytes()
        {
            return size() * bytesPerElement;
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
     *  \struct Channel
     *
     *  \brief Channel parameters sizes and locations
     *
     *  Paramters that define the Channel signal array and
     *  PVP array size and location.
     *  See section 2.4 and 2.5
     */
    // There's another Channel type in the Metadata,
    // so hiding this inside Data
    struct SIX_CPHD_API Channel
    {
        /*
         *  \func Channel
         *
         *  \brief Default constructor
         */
        Channel();

        /*
         *  \func Channel
         *
         *  \brief Custom constructor
         *
         *  \param vectors Set number of vectors
         *  \param samples Set number of samples
         */
        Channel(size_t vectors, size_t samples);

        /*
         *  \func Channel
         *
         *  \brief Custom constructor
         *
         *  \param vectors Set number of vectors
         *  \param samples Set number of samples
         *  \param signalByteOffset set the byte offset of signal block
         *  \param pvpByteOffset set the byte offset of pvp block
         *  \param compressedSize (Optional) Size of the compressed signal block
         */
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

    //! Destructor
    virtual ~Data()
    {
    }

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
    virtual size_t getNumVectors(size_t channel) const;
    virtual size_t getNumSamples(size_t channel) const;
    size_t getCompressedSignalSize(size_t channel) const;
    size_t getSignalSize(size_t channel) const;
    virtual size_t getNumChannels() const
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
    SignalArrayFormat getSampleType() const
    {
        return signalArrayFormat;
    }
    virtual size_t getNumBytesPerSample() const
    {
        return cphd::getNumBytesPerSample(signalArrayFormat);
    }
    size_t getNumSupportArrays() const
    {
        return supportArrayMap.size();
    }

    // SupportArray get functions
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
                         int64_t offset);

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

    //! (Optional) Unordered map key: unique identifier string,
    //! value: supportArray object
    std::unordered_map<std::string, SupportArray> supportArrayMap;

private:
    // Custom comparator
    struct CmpByOffset
    {
        // Overload operator ()
        // Returns true if offset lhs is less than other offset
        bool operator()(const int64_t& lhs, const int64_t& rhs) const
        {
            return lhs < rhs;
        }
    };

    /*
     * Check if channel is in range
     */
    void verifyChannelInRange(size_t channel) const;

    // Book keeping map for efficient validation
    // Support Array Map with:
    // key: offset, value: array size
    std::map<int64_t, size_t, CmpByOffset> mOffsetMap;
};

//! Ostream operators
std::ostream& operator<< (std::ostream& os, const Data::SupportArray& s);
std::ostream& operator<< (std::ostream& os, const Data::Channel& c);
std::ostream& operator<< (std::ostream& os, const Data& d);
}

#endif

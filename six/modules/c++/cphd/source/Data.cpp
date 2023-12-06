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
#include <map>
#include <cphd/Data.h>
#include <six/Init.h>

namespace cphd
{

Data::Data() :
    numBytesPVP(six::Init::undefined<size_t>())
{
}

bool Data::isCompressed() const
{
    if (six::Init::isUndefined<std::string>(signalCompressionID))
    {
        return false;
    }
    return true;
}

Data::Channel::Channel() :
    numVectors(0),
    numSamples(0),
    signalArrayByteOffset(0),
    pvpArrayByteOffset(0),
    compressedSignalSize(six::Init::undefined<size_t>())
{
}

Data::Channel::Channel(size_t vectors, size_t samples) :
    numVectors(vectors),
    numSamples(samples),
    signalArrayByteOffset(0),
    pvpArrayByteOffset(0),
    compressedSignalSize(six::Init::undefined<size_t>())
{
}

Data::Channel::Channel(size_t vectors, size_t samples,
        size_t signalByteOffset, size_t pvpByteOffset,
        size_t compressedSize) :
    numVectors(vectors),
    numSamples(samples),
    signalArrayByteOffset(signalByteOffset),
    pvpArrayByteOffset(pvpByteOffset),
    compressedSignalSize(compressedSize)
{
}

Data::SupportArray::SupportArray() :
    numRows(0),
    numCols(0),
    bytesPerElement(0),
    arrayByteOffset(0)
{
}

Data::SupportArray::SupportArray(const std::string& id, size_t rows, size_t cols,
                   size_t numBytes, size_t offset) :
    identifier(id),
    numRows(rows),
    numCols(cols),
    bytesPerElement(numBytes),
    arrayByteOffset(offset)
{
}

Data::SupportArray Data::getSupportArrayById(const std::string& id) const
{
    if(supportArrayMap.count(id) != 1)
    {
        std::ostringstream ostr;
        ostr << "ID: " << id << " is not a valid identifier";
        throw except::Exception(Ctxt(ostr));
    }
    return supportArrayMap.find(id)->second;
}

void Data::setSupportArray(const std::string& id, size_t numRows,
                           size_t numCols, size_t numBytes,
                           int64_t offset)
{
    if (supportArrayMap.count(id))
    {
        std::ostringstream ostr;
        ostr << "Identifier " << id << " is not unique";
        throw except::Exception(Ctxt(ostr));
    }

    if (mOffsetMap.count(offset))
    {
        std::ostringstream ostr;
        ostr << "Offset " << offset << " is not unique";
        throw except::Exception(Ctxt(ostr));
    }

    // Add to ordered map
    mOffsetMap.insert(std::pair<int64_t,size_t>(offset,numBytes));

    // Validate offset and size
    if (mOffsetMap.find(offset) != mOffsetMap.begin())
    {
        if (offset < (int64_t)((--mOffsetMap.find(offset))->first + (--mOffsetMap.find(offset))->second))
        {
            std::ostringstream ostr;
            ostr << "Invalid size or offset of support array given for id: " << id;
            throw except::Exception(Ctxt(ostr));
        }
    }
    if (mOffsetMap.upper_bound(offset) != mOffsetMap.end())
    {
        if ((int64_t)(offset + (numRows * numCols * numBytes)) > mOffsetMap.upper_bound(offset)->first)
        {
            std::ostringstream ostr;
            ostr << "Invalid size or offset of support array given for id: " << id;
            throw except::Exception(Ctxt(ostr));
        }
    }

    // Add to supportArrayMap after validation
    std::pair<std::string,Data::SupportArray> entry(id,Data::SupportArray(id, numRows, numCols, numBytes, offset));
    supportArrayMap.insert(entry);
}

//! Getter functions
size_t Data::getNumVectors(size_t channel) const
{
    verifyChannelInRange(channel);
    return channels[channel].getNumVectors();
}
size_t Data::getNumSamples(size_t channel) const
{
    verifyChannelInRange(channel);
    return channels[channel].getNumSamples();
}
size_t Data::getCompressedSignalSize(size_t channel) const
{
    verifyChannelInRange(channel);
    return channels[channel].getCompressedSignalSize();
}
size_t Data::getSignalSize(size_t channel) const
{
    verifyChannelInRange(channel);
    return getNumVectors(channel) *
           getNumSamples(channel) *
           getNumBytesPerSample();
}

size_t Data::getAllSupportSize() const
{
    size_t size = 0;
    for (auto it = supportArrayMap.begin(); it != supportArrayMap.end(); ++it)
    {
        size += getSupportArrayById(it->first).getSize();
    }
    return size;
}

/*
 * Check if channel is in range
 */
void Data::verifyChannelInRange(size_t channel) const
{
    if (channel >= channels.size())
    {
    std::ostringstream ostr;
    ostr << "Channel provided is " << channel << "\n"
            << "while only " << channels.size() << " channels exist \n";
    throw except::Exception(Ctxt(ostr));
    }
}


std::ostream& operator<< (std::ostream& os, const Data::SupportArray& s)
{
    os << "    Identifier        : " << s.identifier << "\n"
        << "    NumRows        : " << s.numRows << "\n"
        << "    NumCols        : " << s.numCols << "\n"
        << "    BytesPerElement : " << s.bytesPerElement << "\n"
        << "    ArrayByteOffset : " << s.arrayByteOffset << "\n";
    return os;
}

std::ostream& operator<< (std::ostream& os, const Data::Channel& c)
{
    os << "  Channel:: \n"
        << "    Identifier     : " << c.identifier << "\n"
        << "    NumVectors     : " << c.numVectors << "\n"
        << "    NumSamples     : " << c.numSamples << "\n"
        << "    SignalArrayByteOffset : " << c.signalArrayByteOffset << "\n"
        << "    PVPArrayByteOffset : " << c.pvpArrayByteOffset << "\n"
        << "    CompressedSignalSize : " << c.compressedSignalSize << "\n";
    return os;
}

std::ostream& operator<< (std::ostream& os, const Data& d)
{
    os << "Data:: \n"
        << "  SignalArrayFormat : " << d.signalArrayFormat << "\n"
        << "  NumBytesPVP      : " << d.numBytesPVP << "\n";
    for (size_t ii = 0; ii < d.channels.size(); ++ii)
    {
        os << d.channels[ii] << "\n";
    }
    os << "  SignalCompressionID : " << d.signalCompressionID << "\n";
    for (auto it = d.supportArrayMap.begin(); it != d.supportArrayMap.end(); ++it)
    {
        os << "  SupportArrays:: \n"
            << d.getSupportArrayById(it->first) << "\n";
    }
    return os;
}
}

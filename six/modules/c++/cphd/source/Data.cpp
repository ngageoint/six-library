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

#include <cphd/Data.h>
#include <six/Init.h>

namespace cphd
{
Data::Data() :
    numBytesPVP(0)
{
}

Data::Channel::Channel() :
    numVectors(0),
    numSamples(0),
    signalArrayByteOffset(0),
    pvpArrayByteOffset(0),
    compressedSignalSize(six::Init::undefined<size_t>())
{
}

Data::SupportArray::SupportArray() :
    numRows(0),
    numCols(0),
    bytesPerElement(0),
    arrayByteOffset(0)
{
}
std::ostream& operator<< (std::ostream& os, const Data::SupportArray& s)
{
    os << "  SupportArray:: \n"
        << "    Identifier     : " << s.identifier << "\n"
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
    for (size_t i = 0; i < d.channels.size(); ++i)
    {
        os << d.channels[i] << "\n";
    }
    os << "  SignalCompressionID : " << d.signalCompressionID << "\n";
    for (size_t i = 0; i < d.supportArrays.size(); ++i)
    {
        os << d.supportArrays[i] << "\n";
    }
    return os;
}


}

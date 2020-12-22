/* =========================================================================
 * This file is part of cphd03-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * cphd03-c++ is free software; you can redistribute it and/or modify
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

#include <six/Init.h>
#include <cphd03/Utilities.h>
#include <cphd03/Data.h>

namespace cphd03
{
ArraySize::ArraySize(size_t v, size_t s) :
    numVectors(v),
    numSamples(s)
{
}

std::ostream& operator<< (std::ostream& os, const ArraySize& d)
{
    os << "ArraySize::\n"
       << "  NumVectors: " << d.numVectors << "\n"
       << "  NumSamples: " << d.numSamples << "\n";
     return os;
}

Data::Data() :
    sampleType(cphd::SampleType::NOT_SET),
    numCPHDChannels(six::Init::undefined<size_t>()),
    numBytesVBP(six::Init::undefined<size_t>())
{
}

size_t Data::getNumVectors(size_t chan) const
{
    if (chan >= numCPHDChannels)
    {
        return 0;
    }

    return arraySize.at(chan).numVectors;
}

size_t Data::getNumSamples(size_t chan) const
{
    if (chan >= numCPHDChannels)
    {
        return 0;
    }

    return arraySize.at(chan).numSamples;
}

size_t Data::getNumBytesPerSample() const
{
    return cphd03::getNumBytesPerSample(sampleType);
}

std::ostream& operator<< (std::ostream& os, const Data& d)
{
    os << "Data::" << "\n"
       << "  SampleType     : " << d.sampleType << "\n"
       << "  NumCPHDChannels: " << d.numCPHDChannels << "\n"
       << "  NumBytesVBP    : " << d.numBytesVBP << "\n";

    for (size_t ii = 0; ii < d.arraySize.size(); ++ii)
    {
        os << "  [" << (ii) << "] " << d.arraySize[ii] << "\n";
    }

    return os;
}
}

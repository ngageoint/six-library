/******************************************************************************
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * Developed in the performance of one or more U.S. Government contracts.
 * The U.S. Government has Unlimited Rights in this computer software as set
 * forth in the Rights in Technical Data and Compute Software: Noncommercial
 * Items clauses.
 ******************************************************************************/

#include <six/Init.h>
#include <cphd/Utilities.h>
#include <cphd/Data.h>

namespace cphd
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
    sampleType(SampleType::NOT_SET),
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
    return cphd::getNumBytesPerSample(sampleType);
}

std::ostream& operator<< (std::ostream& os, const Data& d)
{
    os << "Data::" << "\n"
       << "  SampleType     : " << d.sampleType.toString() << "\n"
       << "  NumCPHDChannels: " << d.numCPHDChannels << "\n"
       << "  NumBytesVBP    : " << d.numBytesVBP << "\n";

    for (size_t ii = 0; ii < d.arraySize.size(); ++ii)
    {
        os << "  [" << (ii) << "] " << d.arraySize[ii] << "\n";
    }

    return os;
}
}

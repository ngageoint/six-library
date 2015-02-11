/******************************************************************************
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * Developed in the performance of one or more U.S. Government contracts.
 * The U.S. Government has Unlimited Rights in this computer software as set
 * forth in the Rights in Technical Data and Compute Software: Noncommercial
 * Items clauses.
 ******************************************************************************/


#ifndef __CPHD_DATA_H__
#define __CPHD_DATA_H__

#include <ostream>
#include <vector>
#include <stddef.h>

#include <cphd/Enums.h>

namespace cphd
{
struct ArraySize
{
    ArraySize(size_t v = 0, size_t s = 0);

    bool operator==(const ArraySize& other) const
    {
        return numVectors == other.numVectors &&
               numSamples == other.numSamples;
    }

    bool operator!=(const ArraySize& other) const
    {
        return !((*this) == other);
    }

    size_t numVectors;
    size_t numSamples;
};

std::ostream& operator<< (std::ostream& os, const ArraySize& d);

struct Data
{
    Data();

    size_t getNumChannels() const
    {
        return numCPHDChannels;
    }

    // 1-based channel number
    size_t getNumVectors(size_t channel) const;

    // 1-based channel number
    size_t getNumSamples(size_t channel) const;

    size_t getNumBytesPerSample() const;

    size_t getNumBytesVBP() const
    {
        return numBytesVBP;
    }

    void setNumBytesVBP(size_t n)
    {
        numBytesVBP = n;
    }

    bool operator==(const Data& other) const
    {
        return sampleType == other.sampleType &&
               numCPHDChannels == other.numCPHDChannels &&
               numBytesVBP == other.numBytesVBP &&
               arraySize == other.arraySize;
    }

    bool operator!=(const Data& other) const
    {
        return !((*this) == other);
    }

    SampleType sampleType;
    size_t numCPHDChannels;
    size_t numBytesVBP;

    std::vector<ArraySize> arraySize;
};

std::ostream& operator<< (std::ostream& os, const Data& d);
}

#endif

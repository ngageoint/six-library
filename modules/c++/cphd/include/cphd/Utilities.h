/******************************************************************************
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * Developed in the performance of one or more U.S. Government contracts.
 * The U.S. Government has Unlimited Rights in this computer software as set
 * forth in the Rights in Technical Data and Compute Software: Noncommercial
 * Items clauses.
 ******************************************************************************/


#ifndef __CPHD_UTILITIES_H__
#define __CPHD_UTILITIES_H__

#include <str/Manip.h>
#include <cphd/Enums.h>
#include <cphd/Types.h>

namespace cphd
{
// Return bytes/sample, either 2, 4, or 8 (or 0 if not initialized)
size_t getNumBytesPerSample(cphd::SampleType sampleType);

template<typename T> std::string toString(const T& value)
{
    return str::toString<T>(value);
}

template<> std::string toString(const CollectionInformation& ci);
}

#endif

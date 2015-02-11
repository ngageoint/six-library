/******************************************************************************
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * Developed in the performance of one or more U.S. Government contracts.
 * The U.S. Government has Unlimited Rights in this computer software as set
 * forth in the Rights in Technical Data and Compute Software: Noncommercial
 * Items clauses.
 ******************************************************************************/


#ifndef __CPHD_METADATA_H__
#define __CPHD_METADATA_H__

#include <ostream>

#include <cphd/Types.h>
#include <cphd/Data.h>
#include <cphd/Global.h>
#include <cphd/Channel.h>
#include <cphd/SRP.h>
#include <cphd/Antenna.h>
#include <cphd/VectorParameters.h>

namespace cphd
{
/*!
 *  \class Metadata
 *
 *  This class extends the Data object providing the CPHD
 *  layer and utilities for access.  In order to write a CPHD,
 *  you must have a CPHDData object that is populated with
 *  the mandatory parameters, and you must add it to the Container
 *  object first.
 *
 *  This object contains all of the sub-blocks for CPHD.
 *
 *
 */
struct Metadata
{
    Metadata()
    {
    }

    void setSampleType(SampleType sampleType)
    {
        data.sampleType = sampleType;
    }

    SampleType getSampleType() const
    {
        return data.sampleType;
    }

    size_t getNumChannels() const;
    size_t getNumVectors(size_t channel) const;   // 0-based channel number
    size_t getNumSamples(size_t channel) const;   // 0-based channel number
    size_t getNumBytesPerSample() const;          // 2, 4, or 8 bytes/complex sample

    bool isFX() const
    {
        return (getDomainType() == DomainType::FX);
    }

    bool isTOA() const
    {
        return (getDomainType() == DomainType::TOA);
    }
    
    // returns "FX", "TOA", or "NOT_SET"
    std::string getDomainTypeString() const;

    // returns enum for FX, TOA, or NOT_SET
    cphd::DomainType getDomainType() const;

    //!  CollectionInfo block.  Contains the general collection information
    //!  CPHD can use the SICD Collection Information block directly
    CollectionInformation collectionInformation;

    //!  Data block. Very unfortunate name, but matches the CPHD spec.
    //!  Contains the  information
    cphd::Data data;

    //!  Global block. Contains the information
    Global global;

    //! Channel block. Contains the information
    Channel channel;

    //!  SRP block.  Contains the  information
    SRP srp;

    //!  Optional Antenna block.  Contains the  information
    //!  This is similar to, but not identical to, the SICD Antenna
    mem::ScopedCopyablePtr<Antenna> antenna;

    //!  VectorParameters block.  Contains the  information
    VectorParameters vectorParameters;

    bool operator==(const Metadata& other) const;

    bool operator!=(const Metadata& other) const
    {
        return !((*this) == other);
    }
};

std::ostream& operator<< (std::ostream& os, const Metadata& d);
}

#endif

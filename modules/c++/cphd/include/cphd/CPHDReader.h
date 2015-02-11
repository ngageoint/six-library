/******************************************************************************
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * Developed in the performance of one or more U.S. Government contracts.
 * The U.S. Government has Unlimited Rights in this computer software as set
 * forth in the Rights in Technical Data and Compute Software: Noncommercial
 * Items clauses.
 ******************************************************************************/


#ifndef __CPHD_CPHD_READER_H__
#define __CPHD_CPHD_READER_H__

#include <memory>

#include <sys/Conf.h>
#include <cphd/Metadata.h>
#include <cphd/FileHeader.h>
#include <cphd/VBM.h>
#include <cphd/Wideband.h>

namespace cphd
{
class CPHDReader
{
public:
    //!  Constructor
    // Provides access to wideband but doesn't read it
    CPHDReader(mem::SharedPtr<io::SeekableInputStream> inStream,
               size_t numThreads,
               mem::SharedPtr<logging::Logger> logger =
                       mem::SharedPtr<logging::Logger>());

    CPHDReader(const std::string& fromFile,
               size_t numThreads,
               mem::SharedPtr<logging::Logger> logger =
                       mem::SharedPtr<logging::Logger>());

    size_t getNumChannels() const
    {
        return mMetadata->getNumChannels();
    }

    // 0-based channel number
    size_t getNumVectors(size_t channel) const
    {
        return mMetadata->getNumVectors(channel);
    }

    // 0-based channel number
    size_t getNumSamples(size_t channel) const
    {
        return mMetadata->getNumSamples(channel);
    }

    // returns total per complex sample (2, 4, or 8)
    size_t getNumBytesPerSample() const
    {
        return mMetadata->getNumBytesPerSample();
    }

    // Return offset from start of CPHD file for a vector and sample for a channel
    // first channel is 0!
    // 0-based vector in channel
    // 0-based sample in channel
    sys::Off_T getFileOffset(size_t channel, size_t vector, size_t sample) const
    {
        return mWideband->getFileOffset(channel, vector, sample);
    }

    bool isFX() const
    {
        return (getDomainType() == DomainType::FX);
    }

    bool isTOA() const
    {
        return (getDomainType() == DomainType::TOA);
    }

    // returns "FX", "TOA", or "NOT_SET"
    std::string getDomainTypeString() const
    {
        return std::string(getDomainType());
    }

    // returns enum for FX, TOA, or NOT_SET
    cphd::DomainType getDomainType() const
    {
        return mMetadata->getDomainType();
    }

    // Functions required to access Header, Metadata, VBP and PH data
    const FileHeader& getFileHeader() const
    {
        return mFileHeader;
    }

    const Metadata& getMetadata() const
    {
        return *mMetadata;
    }

    const VBM& getVBM() const
    {
        return *mVBM;
    }

    Wideband& getWideband()
    {
        return *mWideband;
    }

private:
    // Keep info about the CPHD collection
    FileHeader mFileHeader;
    std::auto_ptr<Metadata> mMetadata;
    std::auto_ptr<VBM> mVBM;
    std::auto_ptr<Wideband> mWideband;

    void initialize(mem::SharedPtr<io::SeekableInputStream> inStream,
                    size_t numThreads,
                    mem::SharedPtr<logging::Logger> logger);

};
}

#endif

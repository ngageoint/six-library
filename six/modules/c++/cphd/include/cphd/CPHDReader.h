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
#ifndef SIX_cphd_CPHDReader_h_INCLUDED_
#define SIX_cphd_CPHDReader_h_INCLUDED_

#include <memory>

#include <scene/sys_Conf.h>

#include <cphd/Metadata.h>
#include <cphd/FileHeader.h>
#include <cphd/PVPBlock.h>
#include <cphd/Wideband.h>
#include <cphd/SupportBlock.h>
#include <cphd/Exports.h>

namespace cphd
{
/*
 *  \class CPHDReader
 *
 *  \brief Used to read a CPHD file.
 *  Requires a valid CPHD file,and optional schemas
 *  for XML format verification
 */
struct SIX_CPHD_API CPHDReader final
{
    /*
     *  \func CPHDReader constructor
     *  \brief Construct CPHDReader from an input stream
     *
     *  \param inStream Input stream containing CPHD file
     *  \param numThreads Number of threads for parallelization
     *  \param schemaPaths (Optional) XML schemas for validation
     *  \param logger (Optional) Provide custom log
     */
    // Provides access to wideband but doesn't read it
    CPHDReader(std::shared_ptr<io::SeekableInputStream> inStream,
               size_t numThreads,
               const std::vector<std::string>& schemaPaths =
                       std::vector<std::string>(),
               std::shared_ptr<logging::Logger> logger =
                       std::shared_ptr<logging::Logger>());

    /*
     *  \func CPHDReader constructor
     *  \brief Construct CPHDReader from a file pathname
     *
     *  \param fromFile File path of CPHD file
     *  \param numThreads Number of threads for parallelization
     *  \param schemaPaths (Optional) XML schemas for validation
     *  \param logger (Optional) Provide custom log
     */
    // Provides access to wideband but doesn't read it
    CPHDReader(const std::string& fromFile,
               size_t numThreads,
               const std::vector<std::string>& schemaPaths =
                       std::vector<std::string>(),
               std::shared_ptr<logging::Logger> logger =
                       std::shared_ptr<logging::Logger>());

    CPHDReader(const CPHDReader&) = delete;
    CPHDReader& operator=(const CPHDReader&) = delete;
    CPHDReader(CPHDReader&&) = default;
    CPHDReader& operator=(CPHDReader&&) = default;

    //! Get parameter functions
    size_t getNumChannels() const
    {
        return mMetadata.data.getNumChannels();
    }
    //! 0-based channel number
    size_t getNumVectors(size_t channel) const
    {
        return mMetadata.data.getNumVectors(channel);
    }
    //! 0-based channel number
    size_t getNumSamples(size_t channel) const
    {
        return mMetadata.data.getNumSamples(channel);
    }
    //! returns total per complex sample (2, 4, or 8)
    size_t getNumBytesPerSample() const
    {
        return mMetadata.data.getNumBytesPerSample();
    }

    /*
     *  \func getFileOffset
     *  \brief Calculate signal array offset in file
     *
     *  \param channel The channel number
     *  \param vector The vector number
     *  \param sample The sample number
     *
     *  \return offset of signal sample in file
     */
    int64_t getFileOffset(size_t channel, size_t vector, size_t sample) const
    {
        return mWideband->getFileOffset(channel, vector, sample);
    }

    //! Get file header object
    const FileHeader& getFileHeader() const
    {
        return mFileHeader;
    }
    //! Get metadata object
    const Metadata& getMetadata() const
    {
        return mMetadata;
    }
    //! Get per vector parameters
    const PVPBlock& getPVPBlock() const
    {
        return mPVPBlock;
    }
    //! Get signal data
    const Wideband& getWideband() const
    {
        return *mWideband;
    }
    //! Get support data
    const SupportBlock& getSupportBlock() const
    {
        return *mSupportBlock;
    }

private:
    // Keep info about the CPHD collection
    //! New cphd file header
    FileHeader mFileHeader;
    //! Metadata read in from CPHD file
    Metadata mMetadata;
    //! Support Block book-keeping info read in from CPHD file
    std::unique_ptr<SupportBlock> mSupportBlock;
    //! Per Vector Parameter info read in from CPHD file
    PVPBlock mPVPBlock;
    //! Signal block book-keeping info read in from CPHD file
    std::unique_ptr<Wideband> mWideband;
};
}

#endif // SIX_cphd_CPHDReader_h_INCLUDED_

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
#ifndef __CPHD_WIDEBAND_H__
#define __CPHD_WIDEBAND_H__

#include <string>
#include <complex>

#include <cphd/Data.h>
#include <cphd/Utilities.h>

#include <io/SeekableStreams.h>
#include <mem/BufferView.h>
#include <mem/ScopedArray.h>
#include <mem/SharedPtr.h>
#include <types/RowCol.h>
#include <sys/Conf.h>


namespace cphd
{

//  This class contains information about the Wideband CPHD data.
//  It contains the cphd::Data structure (for channel and vector sizes).
//
//  Due to the large size of CPHD wideband, this object does not contain
//  any actual wideband data
class Wideband
{
public:
    static const size_t ALL;

    /*
     * \param pathname Input CPHD pathname
     * \param data Data section from CPHD
     * \param startWB CPHD header keyword "CPHD_BYTE_OFFSET"
     * \param sizeWB CPHD header keyword "CPHD_DATA_SIZE"
     */
    Wideband(const std::string& pathname,
             const cphd::Data& data,
             sys::Off_T startWB,
             sys::Off_T sizeWB);

    /*
     * \param inStream Input stream to an already opened CPHD file
     * \param data Data section from CPHD
     * \param startWB CPHD header keyword "cphd_BYTE_OFFSET"
     * \param sizeWB CPHD header keyword "cphd_DATA_SIZE"
     */
    Wideband(mem::SharedPtr<io::SeekableInputStream> inStream,
             const cphd::Data& data,
             sys::Off_T startWB,
             sys::Off_T sizeWB);

    // Return offset from start of CPHD file for a vector and sample for a channel
    // first channel is 0!
    // 0-based vector in channel
    // 0-based sample in channel
    sys::Off_T getFileOffset(size_t channel,
                             size_t vector,
                             size_t sample) const;

    // Return offset from start of CPHD file for a vector and sample for a channel
    // first channel is 0!
    // This is specifically for compressed data
    // 0-based vector in channel
    // 0-based sample in channel
    sys::Off_T getFileOffset(size_t channel) const;

    /*
     * Read the specified channel, vector(s), and sample(s)
     * Performs endian swapping if necessary
     *
     * \param channel 0-based channel
     * \param firstVector 0-based first vector to read (inclusive)
     * \param lastVector 0-based last vector to read (inclusive).  Use ALL to
     * read all vectors
     * \param firstSample 0-based first sample to read (inclusive)
     * \param lastSample 0-based last sample to read (inclusive).  Use ALL to
     * read all samples
     * \param numThreads Number of threads to use for endian swapping if
     * necessary
     * \param data Will contain the read in data.  Throws if buffer has not
     * been allocated to a sufficient size
     * (numVectors * numSamples * elementSize)
     */
    void read(size_t channel,
              size_t firstVector,
              size_t lastVector,
              size_t firstSample,
              size_t lastSample,
              size_t numThreads,
              const mem::BufferView<sys::ubyte>& data) const;

    // Same as above for compressed Signal Array
    void read(size_t channel,
              const mem::BufferView<sys::ubyte>& data) const;

    // Same as above but allocates the memory
    void read(size_t channel,
              size_t firstVector,
              size_t lastVector,
              size_t firstSample,
              size_t lastSample,
              size_t numThreads,
              mem::ScopedArray<sys::ubyte>& data) const;

    // Same as above for compressed Signal Array
    void read(size_t channel,
              mem::ScopedArray<sys::ubyte>& data) const;

    // Same as above but also applies a per-vector scale factor
    void read(size_t channel,
              size_t firstVector,
              size_t lastVector,
              size_t firstSample,
              size_t lastSample,
              const std::vector<double>& vectorScaleFactors,
              size_t numThreads,
              const mem::BufferView<sys::ubyte>& scratch,
              const mem::BufferView<std::complex<float> >& data) const;

    // Same as above but for a raw pointer
    // The pointer needs to be preallocated. Use getBufferDims for this.
    void read(size_t channel,
              size_t firstVector,
              size_t lastVector,
              size_t firstSample,
              size_t lastSample,
              size_t numThreads,
              const types::RowCol<size_t>& dims,
              void* data)
    {
        const mem::BufferView<sys::ubyte> buffer(
                static_cast<sys::ubyte*>(data),
                dims.area() * mElementSize);
        read(channel, firstVector, lastVector, firstSample,
             lastSample, numThreads, buffer);
    }

    /*
     * Read all channel, vector(s), and sample(s)
     * Performs endian swapping if necessary
     *
     * \param channel 0-based channel
     * \param firstVector 0-based first vector to read (inclusive)
     * \param lastVector 0-based last vector to read (inclusive).  Use ALL to
     * read all vectors
     * \param firstSample 0-based first sample to read (inclusive)
     * \param lastSample 0-based last sample to read (inclusive).  Use ALL to
     * read all samples
     * \param numThreads Number of threads to use for endian swapping if
     * necessary
     * \param data Will contain the read in data.  Throws if buffer has not
     * been allocated to a sufficient size
     * (numChannels * numVectors * numSamples * elementSize)
     */
    void readAll(size_t firstVector,
              size_t lastVector,
              size_t firstSample,
              size_t lastSample,
              size_t numThreads,
              mem::ScopedArray<sys::ubyte>& data) const;

    /*
     * \brief Gets the dimensions of the wideband block to be read
     *
     * \param channel 0-based channel
     * \param firstVector 0-based first vector to read (inclusive)
     * \param lastVector 0-based last vector to read (inclusive).  Use ALL to
     * read all vectors
     * \param firstSample 0-based first sample to read (inclusive)
     * \param lastSample 0-based last sample to read (inclusive).  Use ALL to
     * read all samples
     */
    types::RowCol<size_t> getBufferDims(size_t channel,
                                        size_t firstVector,
                                        size_t lastVector,
                                        size_t firstSample,
                                        size_t lastSample) const
    {
        types::RowCol<size_t> dims;
        checkReadInputs(channel, firstVector, lastVector,
                        firstSample, lastSample, dims);
        return dims;
    }

private:
    // Initialize mOffsets for each array
    // both for uncompressed and compressed data
    void initialize();

    void checkReadInputs(size_t channel,
                         size_t firstVector,
                         size_t& lastVector,
                         size_t firstSample,
                         size_t& lastSample,
                         types::RowCol<size_t>& dims) const;

    void checkChannelInput(size_t channel) const;

    // Just performs the read
    // No allocation, endian swapping, or scaling
    void readImpl(size_t channel,
                  size_t firstVector,
                  size_t lastVector,
                  size_t firstSample,
                  size_t lastSample,
                  void* data) const;

    // Just performs the read fo compressed data
    // No allocation, endian swapping, or scaling
    void readImpl(size_t channel,
                  void* data) const;

    static
    bool allOnes(const std::vector<double>& vectorScaleFactors);

private:
    // Noncopyable
    Wideband(const Wideband& );
    const Wideband& operator=(const Wideband& );

private:
    const mem::SharedPtr<io::SeekableInputStream> mInStream;
    cphd::Data mData;                 // contains numChan, numVectors
    const sys::Off_T mWBOffset;       // offset in bytes to start of wideband
    const size_t mWBSize;             // total size in bytes of wideband
    const size_t mElementSize;        // element size (bytes / complex sample)

    std::vector<sys::Off_T> mOffsets; // Offset to start of each channel

    friend std::ostream& operator<< (std::ostream& os, const Wideband& d);
};
}

#endif

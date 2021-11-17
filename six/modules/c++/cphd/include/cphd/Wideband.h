/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2020, MDA Information Systems LLC
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
#pragma once

#include <complex>
#include <string>
#include <memory>

#include <scene/sys_Conf.h>
#include <cphd/MetadataBase.h>
#include <cphd/Utilities.h>

#include <io/SeekableStreams.h>
#include <mem/BufferView.h>
#include <mem/ScopedArray.h>
#include <sys/Conf.h>
#include <gsl/gsl.h>
#include <types/RowCol.h>

namespace cphd
{
    class FileHeader;

/*
 * \class Wideband
 * \brief Information about the wideband CPHD data
 */
//  It contains the cphd::Data structure (for channel and vector sizes).
//  Provides methods read wideband data from CPHD file/stream
struct Wideband final
{
    static const size_t ALL;

    /*!
     *  \func Wideband
     *
     *  \brief Constructor initializes signal block book keeping
     *
     *  \param pathname Input CPHD pathname
     *  \param metadata Metadata section of CPHD file
     *  \param startWB CPHD header keyword "CPHD_BYTE_OFFSET"
     *  \param sizeWB CPHD header keyword "CPHD_DATA_SIZE"
     */
    Wideband(const std::string& pathname,
             const cphd::MetadataBase& metadata,
             int64_t startWB,
             int64_t sizeWB);

    /*!
     *  \func Wideband
     *
     *  \brief Constructor initializes signal block book keeping
     *
     *  \param inStream Input stream to an already opened CPHD file
     *  \param metadata Metadata section of CPHD file
     *  \param startWB CPHD header keyword "cphd_BYTE_OFFSET"
     *  \param sizeWB CPHD header keyword "cphd_DATA_SIZE"
     */
    Wideband(std::shared_ptr<io::SeekableInputStream> inStream,
             const cphd::MetadataBase& metadata,
             int64_t startWB,
             int64_t sizeWB);

    /*!
     *  \func getFileOffset
     *
     *  \brief Get the byte offset of a specific signal array in the CPHD file
     *
     *  \param channel channel number of signal array
     *  \param vector vector number of signal channel
     *  \param sample sample number in signal vector
     *
     *  \throw except::Exception If invalid channel, vector or sample
     *
     *  \return Returns offset from start of CPHD file
     */
    // first channel is 0!
    // 0-based vector in channel
    // 0-based sample in channel
    int64_t getFileOffset(size_t channel,
                             size_t vector,
                             size_t sample) const;

    /*!
     *  \func getFileOffset
     *
     *  \brief Get the byte offset of a compressed signal block in the CPHD file
     *
     *  \param channel channel number of signal array
     *
     *  \throw except::Exception If invalid channel, vector or sample
     *
     *  \return Returns offset from start of CPHD file
     */
    // This is specifically for compressed data
    // first channel is 0!
    // 0-based vector in channel
    // 0-based sample in channel
    int64_t getFileOffset(size_t channel) const;

    /*!
     *  \func read
     *
     *  \brief Read the specified channel, vector(s), and sample(s)
     *
     *  Performs endian swapping if necessary
     *
     *  \param channel 0-based channel
     *  \param firstVector 0-based first vector to read (inclusive)
     *  \param lastVector 0-based last vector to read (inclusive).  Use ALL to
     *  read all vectors
     *  \param firstSample 0-based first sample to read (inclusive)
     *  \param lastSample 0-based last sample to read (inclusive).  Use ALL to
     *  read all samples
     *  \param numThreads Number of threads to use for endian swapping if
     *  necessary
     *  \param[in,out] data A pre allocated std::span that will hold the
     * data read from the file.
     *
     *  \throw except::Exception If invalid channel, firstVector, lastVector,
     *   firstSample or lastSample
     *  \throw except::Exception If BufferView memory allocated is insufficient
     *  \throw except::Exception If wideband data is compressed
     */
    void read(size_t channel,
              size_t firstVector,
              size_t lastVector,
              size_t firstSample,
              size_t lastSample,
              size_t numThreads,
              const mem::BufferView<sys::ubyte>& data) const;
    void read(size_t channel,
              size_t firstVector,
              size_t lastVector,
              size_t firstSample,
              size_t lastSample,
              size_t numThreads,
              std::span<std::byte> data) const
    {
        mem::BufferView<sys::ubyte> data_(reinterpret_cast<sys::ubyte*>(data.data()), data.size());
        read(channel, firstVector, lastVector, firstSample, lastSample, numThreads, data_);
    }

    /*!
     *  \func read
     *
     *  \brief Read the specified channel's compressed signal block
     *
     *  \param channel 0-based channel
     *  \param[in,out] data A pre allocated std::span that will hold the
     * data read from the file.
     *
     *  \throw except::Exception If invalid channel
     *  \throw except::Exception If BufferView memory allocated is insufficient
     */
    // Same as above for compressed Signal Array
    void read(size_t channel, const mem::BufferView<sys::ubyte>& data) const;
    void read(size_t channel, std::span<std::byte> data) const
    {
        mem::BufferView<sys::ubyte> data_(reinterpret_cast<sys::ubyte*>(data.data()), data.size());
        read(channel, data_);
    }

    /*!
     *  \func read
     *
     *  \brief Read the specified channel, vector(s), and sample(s)
     *
     *  Performs endian swapping if necessary
     *
     *  \param channel 0-based channel
     *  \param firstVector 0-based first vector to read (inclusive)
     *  \param lastVector 0-based last vector to read (inclusive).  Use ALL to
     *  read all vectors
     *  \param firstSample 0-based first sample to read (inclusive)
     *  \param lastSample 0-based last sample to read (inclusive).  Use ALL to
     *  read all samples
     *  \param numThreads Number of threads to use for endian swapping if
     *  necessary
     *  \param[out] data An empty std::unique_ptr<[]> that will hold the data
     *   read from the file.
     *
     *  \throw except::Exception If invalid channel, firstVector, lastVector,
     *   firstSample or lastSample
     *  \throw except::Exception If wideband data is compressed
     */
    // Same as above but allocates the memory
    void read(size_t channel,
              size_t firstVector,
              size_t lastVector,
              size_t firstSample,
              size_t lastSample,
              size_t numThreads,
              std::unique_ptr<sys::ubyte[]>& data) const;
    std::unique_ptr<std::byte[]>read(size_t channel,
              size_t firstVector,
              size_t lastVector,
              size_t firstSample,
              size_t lastSample,
              size_t numThreads) const
    {
        std::unique_ptr<sys::ubyte[]> data;
        read(channel, firstVector, lastVector, firstSample, lastSample, numThreads, data);

        return std::unique_ptr<std::byte[]>(reinterpret_cast<std::byte*>(data.release()));
    }

    /*!
     *  \func read
     *
     *  \brief Read the specified channel's compressed signal block
     *
     *  \param channel 0-based channel
     *  \param[out] data An empty std::unique_ptr<[]> that will hold the data
     *   read from the file.
     *
     *  \throw except::Exception If invalid channel
     *  \throw except::Exception If BufferView memory allocated is insufficient
     */
    // Same as above for compressed Signal Array
    void read(size_t channel, std::unique_ptr<sys::ubyte[]>& data) const;
    void read(size_t channel, std::unique_ptr<std::byte[]>& data) const
    {
        std::unique_ptr<sys::ubyte[]> data_;
        read(channel, data_);
        data.reset(reinterpret_cast<std::byte*>(data_.release()));
    }

    /*!
     *  \func read
     *
     *  \brief Read the specified channel, vector(s), and sample(s)
     *
     *  Performs scaling, promotion of sample, and endian swapping if necessary
     *
     *  \param channel 0-based channel
     *  \param firstVector 0-based first vector to read (inclusive)
     *  \param lastVector 0-based last vector to read (inclusive).  Use ALL to
     *   read all vectors
     *  \param firstSample 0-based first sample to read (inclusive)
     *  \param lastSample 0-based last sample to read (inclusive).  Use ALL to
     *   read all samples
     *  \param vectorScaleFactors A vector of scaleFactors to scale signal
     * samples \param numThreads Number of threads to use for endian swapping if
     *   necessary
     *  \param scratch A pre allocated std::span for scratch space for
     * scaling, promoting and/or byte swapping \param[out] data A pre allocated
     *std::span that will hold the data read from the file.
     *
     *  \throw except::Exception If invalid channel, firstVector, lastVector,
     *   firstSample or lastSample
     *  \throw except::Exception If scaleFactors vector size is not equal to
     * number of samples
     *  \throw except::Exception If scratch size is not
     * at least the bytes size of one signal array
     *  \throw except::Exception If wideband data is compressed
     */
    // Same as above but also applies a per-vector scale factor
    void read(size_t channel,
              size_t firstVector,
              size_t lastVector,
              size_t firstSample,
              size_t lastSample,
              const std::vector<double>& vectorScaleFactors,
              size_t numThreads,
              const mem::BufferView<sys::ubyte>& scratch,
              const mem::BufferView<std::complex<float>>& data) const;
    void read(size_t channel,
              size_t firstVector,
              size_t lastVector,
              size_t firstSample,
              size_t lastSample,
              const std::vector<double>& vectorScaleFactors,
              size_t numThreads,
              std::span<std::byte> scratch,
              std::span<std::complex<float>> data) const
    {
        mem::BufferView<sys::ubyte> scratch_(reinterpret_cast<sys::ubyte*>(scratch.data()), scratch.size());
        mem::BufferView<std::complex<float>> data_(data.data(), data.size());
        read(channel, firstVector, lastVector, firstSample, lastSample, vectorScaleFactors, numThreads,
            scratch_, data_);
    }

    /*!
     *  \func read
     *
     *  \brief Read the specified channel, vector(s), and sample(s)
     *
     *  Performs endian swapping if necessary
     *
     *  \param channel 0-based channel
     *  \param firstVector 0-based first vector to read (inclusive)
     *  \param lastVector 0-based last vector to read (inclusive).  Use ALL to
     *  read all vectors
     *  \param firstSample 0-based first sample to read (inclusive)
     *  \param lastSample 0-based last sample to read (inclusive).  Use ALL to
     *  read all samples
     *  \param numThreads Number of threads to use for endian swapping if
     *  necessary
     *  \param[out] data A void pointer to a data buffer.
     *
     *  \throw except::Exception If invalid channel, firstVector, lastVector,
     *   firstSample or lastSample
     *  \throw except::Exception If wideband data is compressed
     */
    // Same as above but for a raw pointer
    // The pointer needs to be preallocated. Use getBufferDims for this.
    void read(size_t channel,
              size_t firstVector,
              size_t lastVector,
              size_t firstSample,
              size_t lastSample,
              size_t numThreads,
              const types::RowCol<size_t>& dims,
              void* data) const
    {
        std::span<std::byte> buffer(static_cast<std::byte*>(data),
                                                 dims.area() * mElementSize);
        read(channel,
             firstVector,
             lastVector,
             firstSample,
             lastSample,
             numThreads,
             buffer);
    }

    /*!
     * Calculate the number of bytes required to read requested channel
     * Overload for simply requesting entire channel.
     * \param channel 0-based channel
     * \return Number of bytes
     */
    size_t getBytesRequiredForRead(size_t channel) const;

    /*!
     * Calculate the number of bytes required to read requested
     * portion of wideband data.
     * \param channel 0-based channel
     * \param firstVector 0-based first vector of read request (inclusive)
     * \param lastVector 0-based last vector of read request (inclusive)
     * \param firstSample 0-based first sample of read request (inclusive)
     * \param lastSample 0-based last sample of read request(inclusive)
     * \return Number of bytes in area
     * \throw except::Exception If wideband data is compressed
     */
    size_t getBytesRequiredForRead(size_t channel,
                                   size_t firstVector,
                                   size_t lastVector,
                                   size_t firstSample,
                                   size_t lastSample) const;

    /*!
     *  \func getBufferDims
     *
     *  \brief Gets the uncompressed dimensions of the wideband block to be read
     *
     *  \param channel 0-based channel
     *  \param firstVector 0-based first vector to read (inclusive)
     *  \param lastVector 0-based last vector to read (inclusive).  Use ALL to
     *  read all vectors
     *  \param firstSample 0-based first sample to read (inclusive)
     *  \param lastSample 0-based last sample to read (inclusive).  Use ALL to
     *  read all samples
     */
    types::RowCol<size_t> getBufferDims(size_t channel,
                                        size_t firstVector,
                                        size_t lastVector,
                                        size_t firstSample,
                                        size_t lastSample) const
    {
        types::RowCol<size_t> dims;
        checkReadInputs(channel,
                        firstVector,
                        lastVector,
                        firstSample,
                        lastSample,
                        dims);
        return dims;
    }

    /*!
     * Get sample type element size
     */
    size_t getElementSize() const
    {
        return mElementSize;
    }

private:
    /*
     *  Initialize mOffsets for each array
     *  both for uncompressed and compressed data
     */
    void initialize();

    bool isPartialRead(size_t channel, const types::RowCol<size_t>& dims) const;

    /*
     *  Validate all inputs
     *  Return dimensions of block to be read
     */
    void checkReadInputs(size_t channel,
                         size_t firstVector,
                         size_t& lastVector,
                         size_t firstSample,
                         size_t& lastSample,
                         types::RowCol<size_t>& dims) const;

    /*
     *  Validate channel input (Called by checkReadInputs)
     */
    void checkChannelInput(size_t channel) const;

    /*
     *  Just performs the read
     *  No allocation, endian swapping or scaling
     */
    void readImpl(size_t channel,
                  size_t firstVector,
                  size_t lastVector,
                  size_t firstSample,
                  size_t lastSample,
                  void* data) const;

    /*
     *  Just performs the read for compressed data
     *  No allocation, endian swapping or scaling
     */
    void readImpl(size_t channel, void* data) const;

    /*
     *  Returns true if scale factor vector is all ones
     *  False otherwise.
     */
    static bool allOnes(const std::vector<double>& vectorScaleFactors);

    bool shouldByteSwap() const;

    Wideband(const Wideband&) = delete;
    const Wideband& operator=(const Wideband&) = delete;

private:
    const std::shared_ptr<io::SeekableInputStream> mInStream;
    const cphd::MetadataBase& mMetadata;  // pointer to data metadata
    const int64_t mWBOffset;  // offset in bytes to start of wideband
    const size_t mWBSize;  // total size in bytes of wideband
    const size_t mElementSize;  // element size (bytes / complex sample)

    std::vector<int64_t> mOffsets;  // Offset to start of each channel

    friend std::ostream& operator<<(std::ostream& os, const Wideband& d);
};
}

#endif

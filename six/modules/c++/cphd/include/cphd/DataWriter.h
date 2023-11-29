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
#ifndef SIX_cphd_DataWriter_h_INCLUDED_
#define SIX_cphd_DataWriter_h_INCLUDED_

#include <vector>
#include <std/bit>
#include <std/cstddef> // std::byte
#include <std/span>

#include <io/FileOutputStream.h>
#include <sys/OS.h>
#include <sys/Span.h>

#include "cphd/Exports.h"

namespace cphd
{

/*
 *  \class DataWriter
 *
 *  \brief Class to handle writing to file and byte swapping
 */
struct SIX_CPHD_API DataWriter
{
    /*
     *  \func DataWriter
     *  \brief Constructor
     *
     *  \param stream The seekable output stream to be written
     *  \param numThreads Number of threads for parallel processing
     */
    DataWriter(io::OutputStream&, size_t numThreads);
    
    DataWriter() = delete;
    DataWriter(const DataWriter&) = delete;
    DataWriter& operator=(const DataWriter&) = delete;
    DataWriter(DataWriter&&) = default;
    DataWriter& operator=(DataWriter&&) = default;

    /*
     *  Destructor
     */
    virtual ~DataWriter() = default;

    /*
     *  \func operator()
     *  \brief Overload operator performs write and endian swap
     *
     *  \param data Pointer to the data that will be written to the filestream
     *  \param numElements Total number of elements in array
     *  \param elementSize Size of each element
     */
    virtual void operator()(std::span<const std::byte>, size_t elementSize) = 0;
    void operator()(const void* data_, size_t numElements, size_t elementSize)
    {
        const auto pData = static_cast<const std::byte*>(data_);
        std::span<const std::byte> data(pData, numElements * elementSize);
        (*this)(data, elementSize);
    }

protected:
    //! Output stream of CPHD
    io::OutputStream& mStream;
    //! Number of threads for parallelism
    const size_t mNumThreads;
};

/*
 *  \class DataWriterLittleEndian
 *
 *  \brief Class to handle writing to output stream and byte swapping
 *
 *  For little endian to big endian storage
 */
struct SIX_CPHD_API DataWriterLittleEndian final : public DataWriter
{
    /*
     *  \func DataWriterLittleEndian
     *  \brief Constructor
     *
     *  \param stream The seekable output stream to be written
     *  \param numThreads Number of threads for parallel processing
     *  \param scratchSize Size of buffer to be used for scratch space
     */
    DataWriterLittleEndian(io::OutputStream&, size_t numThreads, size_t scratchSize);
    DataWriterLittleEndian(std::shared_ptr<io::SeekableOutputStream>&, size_t numThreads, size_t scratchSize); // for SWIG
    DataWriterLittleEndian() = delete;
    DataWriterLittleEndian(const DataWriterLittleEndian&) = delete;
    DataWriterLittleEndian& operator=(const DataWriterLittleEndian&) = delete;
    DataWriterLittleEndian(DataWriterLittleEndian&&) = default;
    DataWriterLittleEndian& operator=(DataWriterLittleEndian&&) = default;
    ~DataWriterLittleEndian() = default;

    /*
     *  \func operator()
     *  \brief Overload operator performs write and endian swap
     *
     *  \param data Pointer to the data that will be written to the filestream
     *  \param numElements Total number of elements in array
     *  \param elementSize Size of each element
     */
    void operator()(std::span<const std::byte>, size_t elementSize) override;
    void operator()(const sys::ubyte* data, size_t numElements, size_t elementSize) // for existing SWIG bindings
    {
      DataWriter* const pThis = this;
      (*pThis)(data, numElements, elementSize);
    }

private:
    // Scratch space buffer
    std::vector<std::byte> mScratch;
};

/*
 *  \class DataWriterBigEndian
 *
 *  \brief Class to handle writing to file
 *
 *  No byte swap. Already big endian.
 */
struct SIX_CPHD_API DataWriterBigEndian final : public DataWriter
{
    /*
     *  \func DataWriter
     *  \brief Constructor
     *
     *  \param stream The seekable output stream to be written
     *  \param numThreads Number of threads for parallel processing
     */
    DataWriterBigEndian(io::OutputStream&, size_t numThreads);
    DataWriterBigEndian(std::shared_ptr<io::SeekableOutputStream>&, size_t numThreads); // for SWIG
    DataWriterBigEndian() = delete;
    DataWriterBigEndian(const DataWriterBigEndian&) = delete;
    DataWriterBigEndian& operator=(const DataWriterBigEndian&) = delete;
    DataWriterBigEndian(DataWriterBigEndian&&) = default;
    DataWriterBigEndian& operator=(DataWriterBigEndian&&) = default;
    ~DataWriterBigEndian() = default;

    /*
     *  \func operator()
     *  \brief Overload operator performs write
     *
     *  No endian swapping is necessary. Already Big Endian.
     *
     *  \param data Pointer to the data that will be written to the filestream
     *  \param numElements Total number of elements in array
     *  \param elementSize Size of each element
     */
    void operator()(std::span<const std::byte>, size_t elementSize) override;
    void operator()(const sys::ubyte* data, size_t numElements, size_t elementSize) // for existing SWIG bindings
    {
      DataWriter* const pThis = this;
      (*pThis)(data, numElements, elementSize);
    }
};

// Create the appropriate DataWriter instance using std::endian::native.  There are fancier
// ways of doing this compile-time "if" check, but this is relatively straight-forward to understand;
// C++20 brings `consteval if`.
namespace details
{
    template<std::endian endianness>
    auto make_DataWriter(io::OutputStream&, size_t numThreads, size_t scratchSize);
    template<>
    inline auto make_DataWriter<std::endian::big>(io::OutputStream& stream, size_t numThreads, size_t /*scratchSize*/)
    {
        return std::make_unique<DataWriterBigEndian>(stream, numThreads);
    }
    template<>
    inline auto make_DataWriter<std::endian::little>(io::OutputStream& stream, size_t numThreads, size_t scratchSize)
    {
        return std::make_unique<DataWriterLittleEndian>(stream, numThreads, scratchSize);
    }
}
inline auto make_DataWriter(io::OutputStream& stream, size_t numThreads, size_t scratchSize)
{
    return details::make_DataWriter<std::endian::native>(stream, numThreads, scratchSize);
}

}

#endif // SIX_cphd_DataWriter_h_INCLUDED_

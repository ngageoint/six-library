/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __NITF_BUFFERED_READER_HPP__
#define __NITF_BUFFERED_READER_HPP__
#pragma once

#include <memory>

#include <nitf/coda-oss.hpp>
#include <nitf/CustomIO.hpp>

namespace nitf
{
/*
 *  \class BufferedReader
 *  \brief Can read a buffer in chunks. Those chunks are then
 *         stored in memory and can be accessed as needed.
 *         This can increase performance over several smaller
 *         reads.
 */
class BufferedReader : public CustomIO
{
public:
    /*
     *  \func Constructor
     *  \brief Sets up the internal structure of a BufferedReader.
     *
     *  \param pathname The input pathname to read from.
     *  \param bufferSize The size of each chunk that should be read.
     */
    BufferedReader(const std::string& pathname, size_t bufferSize);

    /*
     *  \func Constructor
     *  \brief Same as above but allows you to pass in a buffer.
     *
     *  \param pathname The input pathname to read from.
     *  \param buffer The buffer used to store chunks.
     *  \param bufferSize The size of each chunk that should be read.
     *  \param adopt If this is true BufferedReader will own 'buffer'.
     */
    BufferedReader(const std::string& pathname,
                   void* buffer,
                   size_t size,
                   bool adopt = false);

    ~BufferedReader() = default;

    BufferedReader(const BufferedReader&) = delete;
    BufferedReader& operator=(const BufferedReader&) = delete;

    size_t getTotalRead() const noexcept
    {
        return mTotalRead;
    }

    size_t getNumBlocksRead() const noexcept
    {
        return mBlocksRead;
    }

    size_t getNumPartialBlocksRead() const noexcept
    {
        return mPartialBlocks;
    }

    //! Time spent reading
    double getTotalWriteTime() const noexcept
    {
        return mElapsedTime;
    }

protected:

    void readImpl(void* buf, size_t size) override;

    void writeImpl(const void* buf, size_t size) override;

    bool canSeekImpl() const override;

    nitf::Off seekImpl(nitf::Off offset, int whence) override;

    nitf::Off tellImpl() const override;

    nitf::Off getSizeImpl() const override;

    int getModeImpl() const override;

    void closeImpl() override;

private:
    void readNextBuffer();

    const nitf::Off mMaxBufferSize;
    const std::unique_ptr<char[]> mScopedBuffer;
    char* const mBuffer;

    nitf::Off mPosition;
    nitf::Off mBufferSize;
    size_t mTotalRead;
    size_t mBlocksRead;
    size_t mPartialBlocks;
    double mElapsedTime;
    mutable sys::File mFile;
    const int64_t mFileLen;
};

}
#endif

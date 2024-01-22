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

#ifndef __NITF_BUFFERED_WRITER_HPP__
#define __NITF_BUFFERED_WRITER_HPP__
#pragma once

#include <memory>

#include <nitf/coda-oss.hpp>
#include <nitf/CustomIO.hpp>
#include "nitf/exports.hpp"

namespace nitf
{
struct NITRO_NITFCPP_API BufferedWriter : public CustomIO
{
    BufferedWriter(const std::string& file, size_t bufferSize);

    BufferedWriter(const std::string& file,
                   char* buffer,
                   size_t size,
                   bool adopt = false);

    ~BufferedWriter();

    BufferedWriter(const BufferedWriter&) = delete;
    BufferedWriter& operator=(const BufferedWriter&) = delete;

    void flushBuffer();

    uint64_t getTotalWritten() const noexcept
    {
        return mTotalWritten;
    }

    uint64_t getNumBlocksWritten() const noexcept
    {
        return mBlocksWritten;
    }

    uint64_t getNumPartialBlocksWritten() const noexcept
    {
        return mPartialBlocks;
    }

    //! Time spent writing to disk in seconds
    double getTotalWriteTime() const noexcept
    {
        return mElapsedTime;
    }

protected:

    void readImpl(void* buf, size_t size) override;

    void writeImpl(const void* buf, size_t size) override;

    bool canSeekImpl() const noexcept override;

    nitf::Off seekImpl(nitf::Off offset, int whence) override;

    nitf::Off tellImpl() const override;

    nitf::Off getSizeImpl() const override;

    int getModeImpl() const noexcept override;

    void closeImpl() override;

private:
    const size_t mBufferSize;
    const std::unique_ptr<char[]> mScopedBuffer;
    char* const mBuffer;

    nitf::Off mPosition;
    uint64_t mTotalWritten;
    uint64_t mBlocksWritten;
    uint64_t mPartialBlocks;
    double mElapsedTime = 0.0;

    // NOTE: This is at the end to give us a chance to adopt the buffer
    //       in ScopedArray in case sys::File's constructor throws
    mutable sys::File mFile;

    void flushBuffer(const char* buf);
};

}
#endif

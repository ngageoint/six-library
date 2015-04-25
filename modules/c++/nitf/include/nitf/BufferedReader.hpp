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

#include <sys/File.h>
#include <mem/ScopedArray.h>
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
                   char* buffer,
                   size_t size,
                   bool adopt = false);

    virtual ~BufferedReader();

    size_t getTotalRead() const
    {
        return mTotalRead;
    }

    size_t getNumBlocksRead() const
    {
        return mBlocksRead;
    }

    size_t getNumPartialBlocksRead() const
    {
        return mPartialBlocks;
    }

    //! Time spent reading
    double getTotalWriteTime()
    {
        return mElapsedTime;
    }

protected:

    virtual void readImpl(void* buf, size_t size);

    virtual void writeImpl(const void* buf, size_t size);

    virtual bool canSeekImpl() const;

    virtual nitf::Off seekImpl(nitf::Off offset, int whence);

    virtual nitf::Off tellImpl() const;

    virtual nitf::Off getSizeImpl() const;

    virtual int getModeImpl() const;

    virtual void closeImpl();

private:
    void readNextBuffer();

    const size_t mBufferSize;
    const mem::ScopedArray<char> mScopedBuffer;
    char* const mBuffer;

    size_t mPosition;
    size_t mTotalRead;
    size_t mBlocksRead;
    size_t mPartialBlocks;
    double mElapsedTime;
    mutable sys::File mFile;
};

}
#endif

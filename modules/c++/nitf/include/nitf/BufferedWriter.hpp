/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
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

#include "nitf/NITFException.hpp"
#include "nitf/System.hpp"
#include "nitf/IOInterface.hpp"
#include "nitf/IOHandle.hpp"

namespace nitf
{

class BufferedWriter : public IOHandle
{

private:
    nitf::Off mBufferSize;
    bool mOwn;
    char *mBuffer;
    nitf::Off mPosition;
    nitf::Off mTotalWritten;
    nitf::Off mBlocksWritten;
    nitf::Off mPartialBlocks;
public:

    BufferedWriter(std::string file, nitf::Off bufferSize) 
        throw(nitf::NITFException)
        : IOHandle(file, NITF_ACCESS_WRITEONLY, NITF_CREATE),
          mBufferSize(bufferSize), mOwn(true), mBuffer(NULL), mPosition(0),
          mTotalWritten(0), mBlocksWritten(0), mPartialBlocks(0)
    {
        mBuffer = new char[static_cast<size_t>(bufferSize)];
    }

    BufferedWriter(std::string file, char *buffer, 
               nitf::Off size, bool adopt = false) 
        throw(nitf::NITFException)
            : IOHandle(file, NITF_ACCESS_WRITEONLY, NITF_CREATE),
              mBufferSize(size), mOwn(adopt),
              mBuffer(buffer), mPosition(0), 
              mTotalWritten(0), mBlocksWritten(0), mPartialBlocks(0)
    {
    }

    ~BufferedWriter();

    virtual void flushBuffer();

    virtual void read(char * buf, size_t size);

    virtual void write(const char * buf, size_t size) 
        throw(nitf::NITFException);

    virtual nitf::Off seek(nitf::Off offset, int whence) 
        throw(nitf::NITFException);

    virtual nitf::Off tell() throw(nitf::NITFException);

    virtual nitf::Off getSize() throw(nitf::NITFException);

    virtual void close();

    virtual nitf::Off getTotalWritten()
    {
        return mTotalWritten;
    }
    virtual nitf::Off getNumBlocksWritten()
    {
        return mBlocksWritten;
    }

    virtual nitf::Off getNumPartialBlocksWritten()
    {
        return mPartialBlocks;
    }


};

}
#endif

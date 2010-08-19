/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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

#ifndef __NITF_MEMORY_IO_HPP__
#define __NITF_MEMORY_IO_HPP__

#include "nitf/NITFException.hpp"
#include "nitf/System.hpp"
#include "nitf/IOInterface.hpp"

/*!
 * \file IOHandle.hpp
 * \brief Contains wrapper implementation for IOHandle
 */

namespace nitf
{

/*!
 *  \class IOHandle
 *  \brief The C++ wrapper of the nitf_IOHandle
 */
class MemoryIO : public IOInterface
{

private:
    nitf::Off mCapacity;
    bool mOwn;
    char *mBuffer;
    nitf::Off mPosition;

public:

    MemoryIO(nitf::Off capacity) throw(nitf::NITFException)
        : mCapacity(capacity), mOwn(true), mPosition(0)
    {
        mBuffer = new char[capacity];
        if (!mBuffer)
            throw nitf::NITFException(Ctxt("Out of memory"));
    }

    MemoryIO(char *buffer, nitf::Off size, bool adopt = false) throw(nitf::NITFException)
            : mCapacity(size), mOwn(adopt), mBuffer(buffer), mPosition(0)
    {
    }

    ~MemoryIO()
    {
        if (mOwn && mBuffer)
            delete [] mBuffer;
    }

    virtual void read(char * buf, size_t size)
    {
        if ((mPosition + (nitf::Off)size) > mCapacity)
            throw nitf::NITFException(Ctxt("Attempting to read past buffer boundary."));
        memcpy(buf, (char*)(mBuffer + mPosition), size);
        mPosition += (nitf::Off)size;
    }

    virtual void write(const char * buf, size_t size) throw(nitf::NITFException)
    {
        if ((mPosition + (nitf::Off)size) > mCapacity)
            throw nitf::NITFException(Ctxt("Attempting to write past buffer boundary."));
        memcpy((char*)(mBuffer + mPosition), buf, size);
        mPosition += (nitf::Off)size;
    }

    virtual nitf::Off seek(nitf::Off offset, int whence) throw(nitf::NITFException)
    {
        switch (whence)
        {
        case NITF_SEEK_CUR:
            if (offset + mPosition > mCapacity)
                throw nitf::NITFException(
                        Ctxt("Attempting to seek past buffer boundary."));
            mPosition += offset;
            break;
        case NITF_SEEK_END:
            throw nitf::NITFException(Ctxt("SEEK_END is unsupported with MemoryIO."));
        case NITF_SEEK_SET:
            if (offset > mCapacity)
                throw nitf::NITFException(
                        Ctxt("Attempting to seek past buffer boundary."));
            mPosition = offset;
            break;
        }
        return mPosition;
    }

    virtual nitf::Off tell() throw(nitf::NITFException)
    {
        return mPosition;
    }

    virtual nitf::Off getSize() throw(nitf::NITFException)
    {
        return mCapacity;
    }

    virtual void close()
    {
        //do nothing
    }
};

}
#endif

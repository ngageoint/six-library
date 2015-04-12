/* =========================================================================
 * This file is part of io-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * io-c++ is free software; you can redistribute it and/or modify
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

#ifndef __IO_SERIALIZABLE_ARRAY_H__
#define __IO_SERIALIZABLE_ARRAY_H__

#include "io/Serializable.h"
#include <import/sys.h>

namespace io
{

/**
 * Serialize an array to/from a stream.
 */
template<typename T>
class SerializableArray : public Serializable
{
public:

    /**
     * \param buf       the data buffer
     * \param offset    the offset (in elements, not bytes) into the buffer
     * \param length    the length (in elements, not bytes) of the buffer
     * \param skip      optional stride to use
     */
    SerializableArray(T* buf, sys::Size_T offset, sys::Size_T length,
                      sys::Size_T skip = 0) :
        mBuf(buf), mOffset(offset), mLength(length), mSkip(skip)
    {
    }

    /**
     * \param buf       the data buffer
     * \param length    the length (in elements, not bytes) of the buffer
     */
    SerializableArray(T* buf, sys::Size_T length) :
        mBuf(buf), mOffset(0), mLength(length), mSkip(0)
    {
    }

    virtual ~SerializableArray()
    {
    }

    void serialize(io::OutputStream& os)
    {
        T* buf = (T*) (mBuf + mOffset);
        if (mSkip == 0)
            os.write((sys::byte*) buf, sizeof(T) * mLength);
        else
        {
            sys::Size_T skip = mSkip + 1;
            for (sys::Size_T i = 0; i < mLength; i += skip, buf += skip)
                os.write((sys::byte*) buf, sizeof(T));
        }
    }

    void deserialize(io::InputStream& is)
    {
        T* buf = (T*) (mBuf + mOffset);
        if (mSkip == 0)
            is.read((sys::byte*) buf, sizeof(T) * mLength);
        else
        {
            sys::Size_T skip = mSkip + 1;
            sys::byte bytes[sizeof(T)];
            for (sys::Size_T i = 0; i < mLength; ++i)
            {
                if (i % skip == 0)
                    is.read((sys::byte*) buf++, sizeof(T));
                else
                    is.read(bytes, sizeof(T));
            }
        }
    }

protected:
    T* mBuf;
    sys::Size_T mOffset, mLength, mSkip;
};
}

#endif

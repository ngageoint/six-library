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

#ifndef __IO_PROXY_STREAMS_H__
#define __IO_PROXY_STREAMS_H__

#include "io/InputStream.h"
#include "io/OutputStream.h"
#include "io/NullStreams.h"

namespace io
{
class ProxyInputStream : public InputStream
{
public:
    ProxyInputStream(InputStream *proxy, bool ownPtr = false) :
        mOwnPtr(ownPtr)
    {
        mProxy.reset(proxy);
    }

    virtual ~ProxyInputStream()
    {
        if (!mOwnPtr)
            mProxy.release();
    }

    virtual sys::Off_T available()
    {
        return mProxy->available();
    }

    virtual sys::SSize_T read(sys::byte* b, sys::Size_T len)
    {
        return mProxy->read(b, len);
    }

    virtual sys::SSize_T readln(sys::byte *cStr,
                                const sys::Size_T strLenPlusNullByte)
    {
        return mProxy->readln(cStr, strLenPlusNullByte);
    }

    virtual int streamTo(OutputStream& soi, long numBytes = IS_END)
    {
        return mProxy->streamTo(soi, numBytes);
    }

    virtual void setProxy(InputStream *proxy, bool ownPtr = false)
    {
        if (!mOwnPtr)
            mProxy.release();
        mProxy.reset(proxy);
        mOwnPtr = ownPtr;
    }

private:
    std::auto_ptr<InputStream> mProxy;
    bool mOwnPtr;
};

/**
 * Proxies to the given OutputStream.
 */
class ProxyOutputStream : public OutputStream
{
public:
    ProxyOutputStream(OutputStream *proxy, bool ownPtr = false) :
        mOwnPtr(ownPtr)
    {
        mProxy.reset(proxy);
    }
    virtual ~ProxyOutputStream()
    {
        if (!mOwnPtr)
            mProxy.release();
    }

    void write(sys::byte b)
    {
        return mProxy->write(b);
    }

    void write(const std::string& str)
    {
        return mProxy->write(str);
    }

    void writeln(const std::string& str)
    {
        return mProxy->writeln(str);
    }

    virtual void write(const sys::byte* b, sys::Size_T len)
    {
        mProxy->write(b, len);
    }

    virtual void flush()
    {
        mProxy->flush();
    }

    virtual void close()
    {
        mProxy->close();
    }

    virtual void setProxy(OutputStream *proxy, bool ownPtr = false)
    {
        if (!mOwnPtr)
            mProxy.release();
        mProxy.reset(proxy);
        mOwnPtr = ownPtr;
    }

private:
    std::auto_ptr<OutputStream> mProxy;
    bool mOwnPtr;

};

/**
 * An output stream that can be enabled/disabled (toggled).
 */
class ToggleOutputStream : public io::ProxyOutputStream
{
public:
    ToggleOutputStream(io::OutputStream *output = NULL, bool ownPtr = false) :
        io::ProxyOutputStream(NULL), mPtr(output),
                mNullStream(new io::NullOutputStream), mOwnPtr(ownPtr)
    {
        setEnabled(mPtr != NULL);
    }

    virtual ~ToggleOutputStream()
    {
        if (mOwnPtr && mPtr)
            delete mPtr;
        if (mNullStream)
            delete mNullStream;
    }

    void setEnabled(bool flag)
    {
        mEnabled = flag && mPtr;
        setProxy(mEnabled ? mPtr : mNullStream, false);
    }

    inline bool isEnabled() const
    {
        return mEnabled;
    }

    void close()
    {
        if (mEnabled && mPtr)
            mPtr->close();
        setEnabled(false);
    }

private:
    io::OutputStream *mPtr, *mNullStream;
    bool mOwnPtr, mEnabled;
};

}

#endif

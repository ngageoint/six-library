/* =========================================================================
 * This file is part of io-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#include "mem/SharedPtr.h"

namespace io
{
class ProxyInputStream: public InputStream
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

    virtual void setProxy(InputStream *proxy, bool ownPtr = false)
    {
        if (!mOwnPtr)
            mProxy.release();
        mProxy.reset(proxy);
        mOwnPtr = ownPtr;
    }

protected:
    virtual sys::SSize_T readImpl(void* buffer, size_t len)
    {
        return mProxy->read(buffer, len);
    }

    mem::auto_ptr<InputStream> mProxy;
    bool mOwnPtr;
};

/**
 * Proxies to the given OutputStream.
 */
struct ProxyOutputStream: public OutputStream
{
    ProxyOutputStream() = default;
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
    ProxyOutputStream(const ProxyOutputStream&) = delete;
    ProxyOutputStream& operator=(const ProxyOutputStream&) = delete;
    ProxyOutputStream(ProxyOutputStream&&) = default;
    ProxyOutputStream& operator=(ProxyOutputStream&&) = default;

    using OutputStream::write;

    virtual void write(const void* buffer, size_t len)
    {
        mProxy->write(buffer, len);
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

protected:
    mem::auto_ptr<OutputStream> mProxy;
    bool mOwnPtr = false;
};

/**
 * An output stream that can be enabled/disabled (toggled).
 */
struct ToggleOutputStream: public io::ProxyOutputStream
{
    ToggleOutputStream() = default;
    ToggleOutputStream(io::OutputStream *output, bool ownPtr = false) :
        io::ProxyOutputStream(nullptr), mPtr(output), mOwnPtr(ownPtr)
    {
    }
    virtual ~ToggleOutputStream()
    {
        if (mOwnPtr && mPtr)
            delete mPtr;
    }
    ToggleOutputStream(const ToggleOutputStream&) = delete;
    ToggleOutputStream& operator=(const ToggleOutputStream&) = delete;
    ToggleOutputStream(ToggleOutputStream&&) = default;
    ToggleOutputStream& operator=(ToggleOutputStream&&) = default;

    void setEnabled(bool flag)
    {
        mEnabled = flag && mPtr;
        setProxy(mEnabled ? mPtr : &mNullStream, false);
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

protected:
    io::OutputStream* mPtr = nullptr;
    io::NullOutputStream mNullStream;
    bool mOwnPtr = false, mEnabled = false;
};

}

#endif

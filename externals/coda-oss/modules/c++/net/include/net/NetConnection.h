/* =========================================================================
 * This file is part of net-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * net-c++ is free software; you can redistribute it and/or modify
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

#ifndef __NET_CONNECTION_H__
#define __NET_CONNECTION_H__
#pragma once

#include "net/Socket.h"
#include "io/BidirectionalStream.h"
#include "sys/SystemException.h"
#include "except/Exception.h"
#include "mem/SharedPtr.h"

/*!
 *  \file NetConnection.h
 *  \brief Concerned with the implemenation of a 2-way stream for sockets
 *
 *  This contains the implementation and interface for a 2-way socket transfer
 *  API.
 */

namespace net
{
/*!
 *  \class NetConnection
 *  \brief The class for reading and writing to a socket
 *
 *  This class takes uses an internal Net handle.
 *  We can write using the OutputStream, and read using 
 *  the InputStream.  Usually, the developer will prefer to use
 *  the SerializableConnection class, to avoid dealing with the byte
 *  transfer layer.
 */
class NetConnection : public io::BidirectionalStream
{
public:
    /*!
     *  Default constructor
     */
    NetConnection()
    {}

    //! we own the ptr after this transaction
    NetConnection(std::unique_ptr<net::Socket>&& socket) : mSocket(socket.release())
    {}
    #if !CODA_OSS_cpp17  // std::auto_ptr removed in C++17
    NetConnection(std::auto_ptr<net::Socket> socket) : mSocket(socket)
    {}
    #endif

    /*!
     *  Copy constructor
     *  \param connection
     */
    NetConnection(const NetConnection& connection)
    {
        mSocket = connection.mSocket;
    }

    /*!
     *  Assignment operator
     *  \param connection
     */
    NetConnection& operator=(const NetConnection& connection)
    {
        if (&connection != this)
        {
            mSocket = connection.mSocket;
        }
        return *this;
    }

    /*!
     *  Destructor
     */
    virtual ~NetConnection()
    {
        try
        {
            close();
        }
        catch(...)
        {
        }
    }

    /*!
     *  Method to open a connection.  Copies a handle to its internal
     *  matter.  Connects the handles to their readers and writers.
     *  \param connection The handle to initialize 
     *  \throw SocketCreationFailedException
     */
    virtual void open(const NetConnection& connection)
    {
        mSocket = connection.mSocket;
    }

    /*!
     *  Close a connection.  This releases the writers/readers and closes
     *  the handle.
     */
    void close()
    {
        mSocket->close();
    }

    /*!
     *  Get the socket by constant reference
     *  \return The socket
     */
    mem::SharedPtr<net::Socket> getSocket() const
    {
        return mSocket;
    }

    /*!
     *  This method defines a given OutputStream. By defining,
     *  this method, you can define the unique attributes of an OutputStream
     *  inheriting class.
     *  \param buffer The byte array to write to the stream
     *  \param len The length of the byte array to write to the stream
     *  \throw IOException
     */
    virtual void write(const void* buffer, size_t len);

    using io::BidirectionalStream::read;
    using io::BidirectionalStream::write;

protected:
    /*!
     *  Read up to len bytes of data from input stream into an array
     *  \param b   Buffer to read into
     *  \param len The length to read
     *  \throw IOException
     *  \return  The number of bytes read, or -1 if eof
     */
    virtual sys::SSize_T readImpl(void* buffer, size_t len);

    //! The socket
    mem::SharedPtr<net::Socket> mSocket;
};

}
#endif


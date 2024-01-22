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

#ifndef __NET_SERIALIZABLE_CONNECTION_H__
#define __NET_SERIALIZABLE_CONNECTION_H__

/*!
 *  \file
 *  \brief File describing interface for writing serializable objects over
 *  the net
 *
 *  The Serializable API is a very useful feature, because it allows us
 *  to make abstractions about data transfers.  Using the API, the developer
 *  can attempt to think in terms of the transfer of objects, rather than
 *  the transfer of bytes.  Using the HttpObject class, for example, the
 *  user can levy the SerializableConnection class to write and read HTTP,
 *  in a document independent fashion.  To send an object using MIME, one
 *  could define a MimeObject, and hand it a Serializable class, and it
 *  could pass itself over the web.  Up until now, with the streams API, we
 *  have concerned ourselves with the efficient transfer of data streams.
 *  Here, we concern ourselves with hiding the streams, and making data
 *  transfer simple and virtually bug free.
 *
 *
 */
#include "io/Serializable.h"
#include "net/NetConnection.h"

namespace net
{
/*!
 * \class SerializableConnection
 * \brief A class for transferring serializable objects over the net
 *
 * This class is used to simplify data transfer into an object-oriented
 * paradigm.  Instead of thinking of HTTP connections, and SOAP connections,
 * and HTML connections, we think of internet connections that handle HTTP
 * documents, SOAP documents, HTML documents, and even tarball data.
 *
 * This also allows us to treat an HTTP message wrapping an object as a 
 * different entity than the object itself, and to serialize to files as well
 * as just sockets. 
 */
class SerializableConnection
{
public:
    /*!
     *  This is the default constructor.  Youll certainly want to set it
     *  to an existing connection.
     */
    SerializableConnection()
    {
    }

    /*!
     *  This is a constructor featuring a TCP/IP connection.
     *  \param connection  A connection to maintain
     */
    SerializableConnection(const NetConnection& connection)
    {
        mConnection.open(connection);
    }

    //! Destructor
    virtual ~SerializableConnection()
    {
    }

    /*!
     *  Close the connection
     */
    void close()
    {
        mConnection.close();
    }

    /*!
     *  This is a read method for serialization.
     *  We want to simply read objects from the net, and 
     *  we want them to know how to unpack themselves.
     *  \param objectToRetrieve  This object will retrieve
     *  itself from the internet
     */
    void read(io::Serializable& objectToRetrieve)
    {
        objectToRetrieve.deserialize(mConnection);
    }

    /*!
     *  Read up to len bytes of data
     *  \param b   Buffer to read into
     *  \param len The length to read
     *  \throw IOException
     *  \return  The number of bytes read, or -1 if eof
     */
    sys::SSize_T read(sys::byte* b, sys::Size_T len)
    {
        return mConnection.read(b, len);
    }
    sys::SSize_T recv(sys::byte* b, sys::Size_T len)
    {
        return read(b, len);
    }

    /*!
     *  This is a write method for serialization.
     *  We want to write objects over the net in a formm
     *  that is "readable" on the other end
     *  \param objectToSend This object will send itself
     *  over the socket
     */
    void write(io::Serializable& objectToSend)
    {
        objectToSend.serialize(mConnection);
    }

    /*!
     *  This method defines a given OutputStream. By defining,
     *  this method, you can define the unique attributes of an OutputStream
     *  inheriting class.
     *  \param b   The byte array to write to the stream
     *  \param len The length of the byte array to write to the stream
     *  \throw IOException
     */
    void write(const sys::byte* b, sys::Size_T len)
    {
        mConnection.write(b, len);
    }
    void send(const sys::byte* b, sys::Size_T len)
    {
        write(b, len);
    }

private:
    NetConnection mConnection;

};
}

#endif


/* =========================================================================
 * This file is part of net.ssl-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * net.ssl-c++ is free software; you can redistribute it and/or modify
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

 
#pragma once 
#ifndef CODA_OSS_net_ssl_SSLConnection_h_INCLUDED_
#define CODA_OSS_net_ssl_SSLConnection_h_INCLUDED_

#include <net/ssl/config.h>
#include "sys/Conf.h"
#if defined(USE_OPENSSL)
#include <net/NetConnection.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

/*!
 *  \file SSLConnection.h
 *  \brief Concerned with the implemenation of a secure 2-way stream for sockets
 *
 *  This contains the implementation and interface for a 2-way socket transfer
 *  API.
 */

namespace net
{
namespace ssl
{
/*!
 *  \class SSLConnection
 *  \brief The class for reading and writing to a socket, 
 *  with SSL
 *
 *  This class takes uses an internal Net handle.
 *  We can write using the OutputStream, and read using 
 *  the InputStream.  Usually, the developer will prefer to use
 *  the SerializableConnection class, to avoid dealing with the byte
 *  transfer layer.
 */
class SSLConnection : public NetConnection
{
public:
    /*!
     *  Default Constructor
     *  \param native  The socket
     *  \param ctx  The SSL context for this socket
     *  \param serverAuth  Flag for server authentication
     *  \param host  The host name in which we are connected
     */
    SSLConnection(std::unique_ptr<net::Socket>&& socket,
                  SSL_CTX* ctx,
                  bool serverAuth = false,
                  const std::string& host = "");

    /*!  
     *  Destructor
     */
    virtual ~SSLConnection();

    /*!
     *  Close the SSL connection
     */
    virtual void close() 
    { 
        if (mSSL != nullptr)
        {
            SSL_shutdown(mSSL);
        } 
        NetConnection::close();
    }

    /*!
     *  Read up to len bytes of data from input stream into an array
     *  \param b   Buffer to read into
     *  \param len The length to read
     *  \throw IOException
     *  \return  The number of bytes read, or -1 if eof
     */
    virtual sys::SSize_T read(sys::byte* b, sys::Size_T len);

    /*!
     *  This method defines a given OutputStream. By defining,
     *  this method, you can define the unique attributes of an OutputStream
     *  inheriting class.
     *  \param b   The byte array to write to the stream
     *  \param len The length of the byte array to write to the stream
     *  \throw IOException
     */
    virtual void write(const sys::byte* b, sys::Size_T len);

    protected:

    /*!
     *  Binds the socket to an SSL object
     *  \param hostName  The host we are connecting to
     */
    void setupSocket(const std::string& hostName);

    /*!
     *  Authenticates the server by verifying its
     *  certificate
     *  \param hostName  The host we are connecting to
     */
    void verifyCertificate(const std::string& hostName);

    //! The SSL object
    SSL * mSSL;

    //! The BIO error object
    BIO * mBioErr;

    //! Flag for doing additional server authentication
    bool mServerAuthentication;

    //! Default Constructor
    SSLConnection(){}

};

}
}

#endif
#endif  // CODA_OSS_net_ssl_SSLConnection_h_INCLUDED_

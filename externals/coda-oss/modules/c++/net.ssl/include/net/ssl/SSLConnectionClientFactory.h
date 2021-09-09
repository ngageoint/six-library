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

#ifndef __NET_SSL_CONNECTION_CLIENT_FACTORY_H__
#define __NET_SSL_CONNECTION_CLIENT_FACTORY_H__
#pragma once

/*!
 *  \file SSLConnectionClientFactory.h
 *  \brief Contains class for creating client socket connections
 *  This class hides the details of client creation.
 */

#include <net/NetConnectionClientFactory.h>
#include <net/ssl/SSLConnection.h>

#include "sys/Conf.h"

namespace net
{
namespace ssl
{
/*!
 *  \class SSLConnectionClientFactory
 *  \brief Spawns SSLConnectionConnection objects
 *  This class creates an SSL client connection and returns it.
 *  The connection can be used to read or write data over
 *  a socket
 */
class SSLConnectionClientFactory : public NetConnectionClientFactory
{
public:

    /*!
     *  Constructor. Create a TCP factory with SSL
     *  \param clientAuth  Client authentication flag
     *  \param keyfile  The private key file
     *  \param password  The password for the keyfile
     *  \param caList  The list of trusted CAs
     *  \param serverAuth  Server authentication flag
     *  \param ciphers  The list of ciphers to use
     */
    SSLConnectionClientFactory(bool clientAuth = false,
                               const std::string& keyfile = "client.pem",
                               const std::string& password = "password",
                               const std::string& caList = "root.pem",
                               bool serverAuth = true, 
                               char* ciphers = nullptr) : 
        mClientAuthentication(clientAuth),
        mKeyfile(keyfile), mPass(password),
        mCAList(caList), 
        mServerAuthentication(serverAuth),
        mCiphers(ciphers)
    { 
        initializeContext(); 
    }
    
    /*!
     *  Destructor
     */
    virtual ~SSLConnectionClientFactory() 
    {
# if defined(USE_OPENSSL)	
        if (mCtx != nullptr)
        {
            SSL_CTX_free(mCtx);
        }
# endif
    }

protected:

    /*!
     *  Initializes the context information for SSL
     *  connections.
     */
    virtual void initializeContext();

    /*!
     * Return a new connection
     * \param toServer The socket for the new connection
     * \return A new SSLConnection
     */
    virtual NetConnection* newConnection(std::unique_ptr<net::Socket>&& toServer);
    #if !CODA_OSS_cpp17  // std::auto_ptr removed in C++17
    virtual NetConnection* newConnection(mem::auto_ptr<net::Socket> toServer);
    #endif

private:
#   if defined(USE_OPENSSL)
    //! The SSL context
    SSL_CTX* mCtx;
#   endif
    //! Flag for client authentication
    bool mClientAuthentication;
    //! The key file (.pem)
    std::string mKeyfile;
    //! The current password
    std::string mPass;
    //! The Trusted CA list (.pem)
    std::string mCAList;
    //! Flag for server authentication
    bool mServerAuthentication;
    //! The cipher list
    char *mCiphers;
};

}
}

#endif

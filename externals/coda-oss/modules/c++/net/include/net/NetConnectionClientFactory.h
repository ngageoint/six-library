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

#ifndef __NET_CONNECTION_CLIENT_FACTORY_H__
#define __NET_CONNECTION_CLIENT_FACTORY_H__
#pragma once

/*!
 *  \file NetConnectionClientFactory.h
 *  \brief Contains class for creating client socket connections
 *  This class hides the details of client creation.
 */

#include "net/URL.h"
#include "net/Sockets.h"
#include "net/NetConnection.h"
#include "net/ClientSocketFactory.h"
#include "sys/Conf.h"

namespace net
{

/*!
 *  \class NetConnectionClientFactory
 *  \brief Spawns NetConnection objects
 *  This class creates a client connection and returns it.
 *  The connection can be used to read or write data over
 *  a socket
 */
class NetConnectionClientFactory
{
public:
    /*!
            *  Default Constructor
     */
    NetConnectionClientFactory()
    {}

    /*!
     *  Destructor
     */
    virtual ~NetConnectionClientFactory()
    {}

    /*!
     * Create a client connection to the url.
     * \param url
     * \return A connection to the url
     * \throw SocketCreationException
     */
    virtual NetConnection * create(const URL& url);

    /*!
     * Create a client connection to the specified socket address

     * \param port
     * \return A connection to the host
     * \throw SocketCreationException
     */

    virtual NetConnection * create(const SocketAddress& address);

    /*!
     *
     *
     */
    virtual NetConnection* newConnection(std::unique_ptr<Socket>&& toServer);
    #if !CODA_OSS_cpp17  // std::auto_ptr removed in C++17
    virtual NetConnection* newConnection(std::auto_ptr<Socket> toServer);
    #endif
    /*!
     * Destroy a spawned connection.
     * \param connection The connection to destroy
     */
    virtual void destroy(NetConnection * connection)
    {
        if (connection != nullptr)
        {
            delete connection;
        }
    }

protected:

    //! The URL for last created connection
    URL mUrl;
};
}

#endif

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

#ifndef __NET_CONNECTION_SERVER_H__
#define __NET_CONNECTION_SERVER_H__
#pragma once

#include "net/NetConnection.h"
#include "net/Sockets.h"
#include "net/ServerSocketFactory.h"
#include "sys/SystemException.h"
#include "net/SingleThreadedAllocStrategy.h"
#include "mem/SharedPtr.h"


/*! \file
 *  \brief Methods for server socket creation.
 *
 *  Hides details of TCP server creation.
 */
namespace net
{

typedef SingleThreadedAllocStrategy DefaultAllocStrategy;

/*!
 *  Class for TCP/IP server creation abstraction.  Allows developer
 *  to create a socket connection that listens on a port for clients.
 *  Has a pure virtual method for handling the clients.
 *
 */
class NetConnectionServer
{
public:
    const static int DEFAULT_BACKLOG = 5;

    //!  Default constructor.
    NetConnectionServer();

    //! Destructor.
    virtual ~NetConnectionServer() 
    {
    }

    /*!
     *  Create a server on the port, with a backlog queue given in the
     *  parameter list
     *  \param portNumber  The port to start the server on.
     *  \param backlog     The number of requests that may be backlogged.
     */
    void create(int portNumber, int backlog = DEFAULT_BACKLOG);

    /*!
     *  Initialize
     *
     *
     */
    void initialize(net::RequestHandlerFactory* requestHandlerFactory,
		    net::AllocStrategy* allocStrategy = NULL);
    //virtual void handleConnection(NetConnection* connection) = 0;

    /*!
     * Gets the port number
     * \return Port number that the server is (or will be) on.
     */
    int getPortNumber()
    {
        return mPortNumber;
    }

    /*!
     * Gets the host name
     * \return Host name that the server is (or will be) on.
     */
    static std::string getHostName();

    /*!
     *  Accept an incoming client connection.
     *  \return A Socket_T representing that client.
     */
    NetConnection* accept();

protected:
    //! The port number we are listening on
    int mPortNumber;
    //! The amount of backlog
    int mBacklog;
    //! The socket we are listening on
    mem::auto_ptr<net::Socket> mSocket;

    mem::auto_ptr<net::AllocStrategy> mAllocStrategy;
};
}

#endif

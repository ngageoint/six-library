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

#ifndef __NET_SERVER_SOCKET_FACTORY_H__
#define __NET_SERVER_SOCKET_FACTORY_H__
#pragma once

#include "mem/SharedPtr.h"
#include "net/Socket.h"

/*!
 *  \file
 *  \brief File containing classes for creating server sockets
 *
 *  This file contains a base socket factory and derived basic utility
 *  implementations for TCP and UDP.
 *
 *
 */

namespace net
{
/*!
 *  \class ServerSocketFactory
 *  \brief Create a server socket
 *
 *  Base class for server socket implementation.  Creates and binds
 *  a socket of the specified protocol.
 *
 */
class ServerSocketFactory
{
public:

    /*!
     *  Default constructor.  One of the notable
     *  behaviors of this constructor is that on Windows, it
     *  it attempts to register the winsock DLL, which it does
     *  using the net::Win32SocketInit() function.  This function,
     *  using an internal Mutex for synchronization, and a double checking
     *  entry makes the appropriate startup call to the DLL, and registers
     *  an exit-time shutdown hook for the DLL.
     *
     */
    ServerSocketFactory(int proto) : mProto(proto)
    {
        NATIVE_SERVERSOCKET_INIT();
    }

    //! Destructor
    virtual ~ServerSocketFactory()
    {}

    /*!
     *  This routine produces a socket of the desired protocol.
     *  Advanced users will need more functionality, perhaps setting
     *  socket options or configuring the socket.  In that case,
     *  the user will either override this class, or perform the options
     *  on the socket subsequent to this function returning.
     *
     *  The extended classes each override this method providing 
     *  customizations useful or required by the selected protocol
     *
     *  \param address Address to establish the socket for
     *  \return The created & bound socket
     */
    virtual mem::auto_ptr<Socket> create(const SocketAddress& address)
    {
        mem::auto_ptr<Socket> s(new Socket(mProto));

        // Bind to this address
        s->bind(address);
        return s;
    }

protected:
    ServerSocketFactory()
    {}
    int mProto;

};

/*!
 *  \class UDPServerSocketFactory
 *  \brief Socket factory specialized for UDP
 *
 *  This class creates a basic UDP server socket.  Essentially,
 *  it just takes on the implementation of its parent class, but
 *  adds in the ability to broadcast across a socket.
 *
 */
class UDPServerSocketFactory : public ServerSocketFactory
{
public:
    //!  Constructor
    UDPServerSocketFactory() : ServerSocketFactory(UDP_PROTO)
    {}

    //!  Destructor
    virtual ~UDPServerSocketFactory()
    {}

    /*!
     *  Create a socket associated with the address given.  Enable
     *  broadcasts just in case.
     *
     *  \param address The socket address we want to associate this
     *  socket with
     *
     *  \return The produced socket
     */
    virtual mem::auto_ptr<Socket> create(const SocketAddress& address)
    {
        mem::auto_ptr<Socket> s(new Socket(mProto));

        // Make sure we're set up for broadcasting if necessary
        int on = 1;
        s->setOption(SOL_SOCKET, SO_BROADCAST, on);

        // Bind to this address
        s->bind(address);
        return s;
    }



};

/*!
 *  \class TCPServerSocketFactory
 *  \brief Factory producing a (passive) TCP socket
 *
 *  This class produces a TCP socket that is bound and
 *  listens with the specified backlog.  
 *
 */
class TCPServerSocketFactory : ServerSocketFactory
{
public:
    //!  Traditional backlog value
    enum { DEFAULT_BACKLOG = 5 };

    /*!
     *  Default constructor for a server socket factory.
     *  The only argument defaults to a very traditional 
     *  (and arbitrary) number.
     *
     *  Modern HTTP servers, for example, would typically set
     *  the backlog much higher than 5, which originally meant
     *  the number of connections (pending + 3-way handshake).
     * 
     *  \param backlog The backlog for the passive socket
     *
     */
    TCPServerSocketFactory(int backlog = DEFAULT_BACKLOG) :
            ServerSocketFactory(TCP_PROTO),
            mBacklog(backlog)
    {}

    //!  Destructor
    virtual ~TCPServerSocketFactory()
    {}

    /*!
     *  The parent will set us up with a bound socket.  Here
     *  we convert our socket to a passive socket by calling
     *  listen().
     *
     */
    virtual mem::auto_ptr<Socket> create(const SocketAddress& address)
    {
        mem::auto_ptr<Socket> s(new Socket(mProto));

        // Reuse socket address (important for most TCP apps)
        int on = 1;
        s->setOption(SOL_SOCKET, SO_REUSEADDR, on);

        s->bind(address);

        // Convert to a passive socket
        s->listen(mBacklog);

        // Return a socket
        return s;
    }

protected:
    int mBacklog;
};


}

#endif

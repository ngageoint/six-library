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

#ifndef __NET_SOCKET_ADDRESS_H__
#define __NET_SOCKET_ADDRESS_H__

#include "net/Sockets.h"

#if defined(WIN32) || defined(_WIN32)
#   include <winsock2.h>
#else
#   include <arpa/inet.h>
#endif


namespace net
{

/*!
 *  \class SocketAddress
 *  \brief Provides an API for socket addresses
 *
 *  The SocketAddress class provides a familiar, simplified
 *  interface for the BSD sockaddr functionality.
 */
class SocketAddress
{
    SockAddrIn_T mAddress;
public:
    // Constructor.  Clear the underlying sockaddr_in
    SocketAddress();

    /*!
     *  Create a SocketAddress from the host and port.
     *  \param host The host id
     *  \param port The port number
     */
    SocketAddress(const std::string& host, int port);

    /*!
     *  Construct the socket address of a port.  This is 
     *  intended mostly for servers, and since the host is 
     *  not given, the INADDR_ANY value is supplied to the
     *  underlying sockaddr_in.
     *
     *  \param port
     */
    SocketAddress(int port);

    //!  Clear the sockaddr_in
    void clear();

    /*!
     *  Set the port #.
     *  \param port The port 
     */
    void setPort(int port);

    void setHost(const std::string& host);

    void set(int port, std::string host = "");

    SockAddrIn_T& getAddress()
    {
        return mAddress;
    }
    const SockAddrIn_T& getAddress() const
    {
        return mAddress;
    }

};
}


#endif

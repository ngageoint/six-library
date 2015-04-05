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

#include <string.h>
#include "net/NetConnectionClientFactory.h"

namespace
{
net::HostEnt_T* getHostByName(const std::string& hostname)
{
    return gethostbyname(hostname.c_str());
}
}

net::NetConnection* net::NetConnectionClientFactory::create(const net::URL& url)
{
    mUrl = url;

    // NOTE: This needs to be constructed prior to getHostByName() so that
    //       its constructor initializes the necessary socket stuff.
    net::TCPClientSocketFactory factory;

    const net::HostEnt_T* const hostEnt = getHostByName(url.getHost());
    if (!hostEnt)
    {
        throw sys::SocketException(Ctxt(
                "net::getHostByName() failed for creation \"" +
                url.toString() + "\""));
    }

    net::SocketAddress sa;
    sa.setPort(url.getPort());
    // Add this to class???
    ::memcpy(&(sa.getAddress().sin_addr.s_addr), hostEnt->h_addr,
             hostEnt->h_length);

    return newConnection(factory.create(sa));
}

net::NetConnection* net::NetConnectionClientFactory::newConnection(
        std::auto_ptr<net::Socket> toServer)
{
    return new net::NetConnection(toServer);
}

net::NetConnection * net::NetConnectionClientFactory::create(
        const net::SocketAddress& address)
{
    return newConnection(net::TCPClientSocketFactory().create(address));

}

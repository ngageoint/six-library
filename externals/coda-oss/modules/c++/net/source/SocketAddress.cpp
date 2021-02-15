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

#ifdef _WIN32
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#include "net/SocketAddress.h"

using namespace net;

SocketAddress::SocketAddress()
{
    clear();
    mAddress.sin_family = AF_INET;
}

SocketAddress::SocketAddress(const std::string& host, int port)
{
    clear();
    set(port, host);
}

SocketAddress::SocketAddress(int port)
{
    clear();
    set(port);
}

//!  Clear the sockaddr_in
void SocketAddress::clear()
{
    ::memset(&mAddress, 0, sizeof(SockAddrIn_T));

}

void SocketAddress::setPort(int port)
{
    mAddress.sin_port = htons(static_cast<u_short>(port));
}

void SocketAddress::setHost(const std::string& host)
{
    if (host.empty())
    {
        mAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
#if defined(WIN32) || defined(_WIN32)
        struct sockaddr saddr;
        int slen = sizeof(saddr);
        struct sockaddr_in *paddr = (struct sockaddr_in *)&saddr;
        (void) WSAStringToAddress((LPSTR)host.c_str(), AF_INET, NULL, &saddr, &slen);
        mAddress.sin_addr = paddr->sin_addr;
#else
        ::inet_pton(AF_INET, host.c_str(), &mAddress.sin_addr);
#endif
    }
}

void SocketAddress::set(int port, std::string host)
{
    mAddress.sin_family = AF_INET;
    setPort(port);
    setHost(host);
}

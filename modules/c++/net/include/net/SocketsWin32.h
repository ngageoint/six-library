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

#pragma once
#ifndef CODA_OSS_net_SocketsWin32_h_INCLUDED_
#define CODA_OSS_net_SocketsWin32_h_INCLUDED_

#include <mutex>
#include <tuple>

#ifndef _WINSOCK2API_ // <winsock2.h> already #include'd

#undef BIGENDIAN // #define'd in <winsock2.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS // '...': Use getaddrinfo() or GetAddrInfoW() instead or define _WINSOCK_DEPRECATED_NO_WARNINGS to disable deprecated API warnings
#include <winsock2.h>
#include <ws2tcpip.h>

#endif // _WINSOCK2API_
#ifndef BIGENDIAN
#error BIGENDIAN should be #defined!
#endif

#include <import/sys.h>


/*!
 *  \file
 *  \brief Handle winsock-level callss
 */
#if !defined(MAXHOSTNAMELEN)
#  define MAXHOSTNAMELEN 256
#endif

#define NATIVE_SERVERSOCKET_INIT() net::Win32SocketInit()
#define NATIVE_CLIENTSOCKET_INIT() net::Win32SocketInit()
#define NATIVE_SOCKET_DESTROY() net::Win32SocketDestroy()
#define NATIVE_SOCKET_ERROR(ERROR) WSAE ## ERROR
#define NATIVE_SOCKET_GETLASTERROR() WSAGetLastError()
#define NATIVE_SOCKET_FAILED(I) (I == INVALID_SOCKET)

namespace net
{
typedef int         SockLen_T;
typedef SOCKET      Socket_T;
typedef HOSTENT     HostEnt_T;
typedef SOCKADDR    SockAddr_T;
typedef SOCKADDR_IN SockAddrIn_T;
typedef char*       ByteBuf_T;
typedef int         BufSize_T;
typedef const SockAddr_T ConnParam2_T;

//! close socket and throw on failure
inline void closeSocketOrThrow(net::Socket_T socket)
{
    if (closesocket(socket) != 0)
    {
        sys::SocketErr err;
        throw except::Exception(
            Ctxt("Socket close failure: " + err.toString()));
    }
}

/*!
 *  Method calls the dll destroy stuff
 *  \return the result of the startup
 */
inline void Win32SocketDestroy()
{
    WSACleanup();
}

/*!
 *  Method calls the dll init stuff
 *  \return The result of the startup
 */
inline void Win32SocketInit()
{
    static std::mutex mutex;
    static bool inited = false;

    if (!inited)
    {
        std::lock_guard<std::mutex> guard(mutex);
        if (!inited)
        {
            inited = true;
            WORD versionRequested = MAKEWORD(1, 1);
            WSADATA wsaData;
            std::ignore = WSAStartup(versionRequested, &wsaData);
            atexit( net::Win32SocketDestroy );
        }
    }

    /*  WORD versionRequested = MAKEWORD(1, 1); */
    /*  WSADATA wsaData; */
    /*  return (WSAStartup(versionRequested, &wsaData)); */
}

}

#endif  // CODA_OSS_net_SocketsWin32_h_INCLUDED_

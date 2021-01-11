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

#ifndef __NET_SOCKETS_WIN32_H__
#define __NET_SOCKETS_WIN32_H__
#pragma once

#undef BIGENDIAN // #define'd in <winsock2.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <import/sys.h>
#ifndef BIGENDIAN
#error BIGENDIAN should be #defined!
#endif

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
    static sys::Mutex mutex;
    static bool inited = false;

    if (!inited)
    {
        mutex.lock();
        if (!inited)
        {
            inited = true;
            WORD versionRequested = MAKEWORD(1, 1);
            WSADATA wsaData;
            WSAStartup(versionRequested, &wsaData);
            atexit( net::Win32SocketDestroy );
        }
        mutex.unlock();
    }

    /*  WORD versionRequested = MAKEWORD(1, 1); */
    /*  WSADATA wsaData; */
    /*  return (WSAStartup(versionRequested, &wsaData)); */
}

}


#endif

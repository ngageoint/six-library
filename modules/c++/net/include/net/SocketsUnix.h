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

#ifndef __NET_SOCKETS_UNIX_H__
#define __NET_SOCKETS_UNIX_H__
/*! \file
 *  \brief Unix-specific translation layer for sockets.
 */

#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <errno.h>
#include "sys/Dbg.h"
#include <import/sys.h>

#define NATIVE_CLIENTSOCKET_INIT() dbg_printf("Initializing client socket\n")
#define NATIVE_SERVERSOCKET_INIT() net::UnixServerInit()
#define NATIVE_SOCKET_DESTROY() dbg_printf("Destroying socket\n")
#define NATIVE_SOCKET_FAILED(I) (I != 0)
#define NATIVE_SOCKET_ERROR(ERROR) E ## ERROR
#define NATIVE_SOCKET_GETLASTERROR() errno
#if !defined(EWOULDBLOCK)
#  define EWOULDBLOCK EAGAIN
#endif

#define INVALID_SOCKET -1

namespace net
{

typedef struct hostent     HostEnt_T;
typedef int                Socket_T;
typedef struct sockaddr    SockAddr_T;
typedef struct sockaddr_in SockAddrIn_T;
typedef const SockAddr_T ConnParam2_T;

#if defined(__linux) || defined(__linux__)
typedef socklen_t SockLen_T;
#elif defined(__APPLE_CC__)
typedef socklen_t SockLen_T;
#else
typedef int SockLen_T;
#endif

typedef char* ByteBuf_T;
typedef int   BufSize_T;

//! close socket and throw on failure
inline void closeSocketOrThrow(net::Socket_T socket)
{
    if (close(socket) != 0)
    {
        sys::SocketErr err;
        throw except::Exception(
            Ctxt("Socket close failure: " + err.toString()));
    }
}

/*!
 *  Unix-specific function to ignore a SIGPIPE.  This prevents
 *  coredumps on pipe breaks
 *
 */
inline void UnixServerInit()
{
    dbg_printf("Initializing server socket\n");

    ::signal(SIGPIPE, SIG_IGN);
    //::sigignore(SIGPIPE);
}
}

#endif

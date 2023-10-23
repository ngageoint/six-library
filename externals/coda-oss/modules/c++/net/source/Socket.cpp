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

#include "net/Socket.h"

#include <std/memory>

net::Socket::Socket(int proto)
{
    mNative = ::socket(AF_INET, proto, 0);
    if (mNative == INVALID_SOCKET)
    {
        throw sys::SocketException(Ctxt("Socket initialization failed"));
    }
}

void net::Socket::listen(int backlog)
{
    // throw on failure
    if (::listen(mNative, backlog) != 0)
    {
        sys::SocketErr err;
        throw sys::SocketException(
            Ctxt("Socket listen failure: " + err.toString()));
    }                               
}

void net::Socket::connect(const net::SocketAddress& address)
{
    // throw on failure
    if (::connect(mNative, 
                  (net::ConnParam2_T *) &(address.getAddress()),
                  (SockLen_T) sizeof(address.getAddress())) != 0)
    {
        sys::SocketErr err;
        throw sys::SocketException(
            Ctxt("Socket connect failure: " + err.toString()));
    }                               
}

void net::Socket::bind(const net::SocketAddress& address)
{
    // throw on failure
    if (::bind(mNative, 
               (const struct sockaddr *) &(address.getAddress()),
               (SockLen_T) sizeof(address.getAddress())) != 0)
    {
        sys::SocketErr err;
        throw sys::SocketException(
            Ctxt("Socket bind failure: " + err.toString()));
    }                               
}

std::unique_ptr<net::Socket> net::Socket::accept(net::SocketAddress& fromClient)
{
    net::SockAddrIn_T& in = fromClient.getAddress();

    net::SockLen_T addrLen = sizeof(in);
    return std::unique_ptr<net::Socket>(new Socket(::accept(mNative, (net::SockAddr_T *) &in, &addrLen), true));
}

size_t net::Socket::recv(void* b, size_t len, int flags)
{
    sys::SSize_T numBytes(0);
    if (len == 0)
        return static_cast<size_t>(-1);

    // recv() takes buffer in as void* on Unix but char* on Windows
    numBytes = ::recv(mNative, static_cast<char*>(b), static_cast<int>(len), flags);

#if defined(__DEBUG_SOCKET)
    std::cout << "========== READ FROM CONNECTION =============" << std::endl;
#endif

    if ((numBytes == -1 && (NATIVE_SOCKET_GETLASTERROR()
            != NATIVE_SOCKET_ERROR(WOULDBLOCK))))
    {
#if defined(__DEBUG_SOCKET)
        std::cout << " Error on read!!!" << std::endl;
        std::cout << "=============================================" << std::endl << std::endl;
#endif

        sys::Err err;
        std::ostringstream oss;
        oss << "When receiving " << str::toString(len) << " bytes: " << 
            err.toString(); 

        throw sys::SocketException(Ctxt(oss.str()));
    }
    else if (numBytes == 0)
    {
#if defined(__DEBUG_SOCKET)
        std::cout << " Zero byte read (End of connection)" << std::endl;
        std::cout << "=============================================" << std::endl << std::endl;
#endif
        return static_cast<size_t>(-1);
    }
#if defined(__DEBUG_SOCKET)
    std::cout << str::Format("Read %d bytes from socket:", numBytes) << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
    std::cout << std::string(b, numBytes) << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
    std::cout << "=============================================" << std::endl << std::endl;
#endif

    return numBytes;
}

size_t net::Socket::recvFrom(net::SocketAddress& address,
                             void* b,
                             size_t len,
                             int flags)
{

    net::SockAddrIn_T& in = address.getAddress();
    net::SockLen_T addrLen = sizeof(in);

    // recvfrom() takes in buffer as void* on Unix but char* on Windows
    sys::SSize_T bytes = ::recvfrom(mNative, static_cast<char*>(b), static_cast<int>(len), flags,
            (struct sockaddr *) &in, &addrLen);
    if (bytes == -1)
    {
        sys::Err err;
        throw sys::SocketException(
            Ctxt("Socket error while receiving bytes: " +  err.toString()));
    }
    
    return bytes;
}

void net::Socket::send(const void* b, size_t len, int flags)
{
    int numBytes(0);
    if (len <= 0)
        return;
#if defined(__DEBUG_SOCKET)
    std::cout << "========== WROTE TO CONNECTION =============" << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
    std::cout << std::string(b, len) << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
    std::cout << "=============================================" << std::endl << std::endl;
#endif

    // send() takes in buffer as const void* on Unix but const char* on Windows
    numBytes = ::send(mNative, static_cast<const char*>(b), static_cast<int>(len), flags);

    if (numBytes == -1 || (sys::Size_T)numBytes != len)
    {
        sys::Err err;
        std::ostringstream oss;
        oss << "Tried sending " << len << " bytes, " <<
                numBytes << " sent: " <<  err.toString();

        throw sys::SocketException(Ctxt(oss.str()));
    }
}

void net::Socket::sendTo(const SocketAddress& address,
                         const void* b,
                         size_t len,
                         int flags)
{
    // sendto() second parameter is const void* on Unix but const char* on
    // Windows
    int numBytes = ::sendto(mNative, static_cast<const char*>(b), static_cast<int>(len), flags,
            (const struct sockaddr *) &(address.getAddress()),
            (net::SockLen_T) sizeof(address.getAddress()));

    // Maybe shouldn't even bother with this
    if (numBytes == -1 || (sys::Size_T)numBytes != len)
    {
        sys::Err err;
        std::ostringstream oss;
        oss << "Tried sending " << len << " bytes, " << numBytes << " sent: "
            <<  err.toString();

        throw sys::SocketException(Ctxt(oss.str()));
    }
}

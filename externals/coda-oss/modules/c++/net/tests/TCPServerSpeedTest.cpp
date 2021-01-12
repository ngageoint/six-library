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

/*!
 *  \file
 *  \brief Server side of TCP speed test
 *
 *  Start this first, then the client
 */

#include <iostream>
#include <stdexcept>
#include <vector>

#include <sys/Path.h>
#include <sys/LocalDateTime.h>
#include <net/ServerSocketFactory.h>

int main(int argc, char** argv)
{
    try
    {
        // Parse the command line
        if (argc != 3)
        {
            std::cerr << "Usage: " << sys::Path::basename(argv[0])
                      << " <port> <buffer size MB>\n\n";
            return 1;
        }
        const int port = str::toType<int>(argv[1]);
        const size_t bufferSize = str::toType<size_t>(argv[2]) * 1024 * 1024;

        net::SocketAddress address(port);
        mem::auto_ptr<net::Socket> listener =
                net::TCPServerSocketFactory().create(address);
        net::SocketAddress clientAddress;
        mem::auto_ptr<net::Socket> client = listener->accept(clientAddress);

        // First the client sends the # of bytes they'll be sending
        sys::Uint64_T numBytes;
        client->recv(&numBytes, sizeof(sys::Uint64_T));

        std::vector<sys::ubyte> bufferVec(std::min<sys::Uint64_T>(numBytes, bufferSize));
        sys::ubyte* const buffer = &bufferVec[0];

        // Then receive all the bytes
        const sys::LocalDateTime start;
        size_t numBytesReceived = 0;
        while (numBytesReceived < numBytes)
        {
            const size_t numBytesToReceive =
                    std::min<sys::Uint64_T>(bufferSize, numBytesReceived - numBytes);

            numBytesReceived += client->recv(buffer, numBytesToReceive);
        }
        const sys::LocalDateTime stop;

        // Send back an ack
        sys::Uint64_T success(1);
        client->send(&success, sizeof(sys::Uint64_T));

        client->close();
        listener->close();

        const double numSec =
                (stop.getTimeInMillis() - start.getTimeInMillis()) / 1000;
        const double numMB = numBytes / (1024.0 * 1024);
        const double mbPerSec = numMB / numSec;
        std::cout << "Received " << numMB << " MB in " << numSec << " sec ("
                  << mbPerSec << " MB / s)\n";

        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    catch (const except::Exception& ex)
    {
        std::cerr << ex.toString() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception\n";
        return 1;
    }
}

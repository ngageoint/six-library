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
 *  \brief Client side of TCP speed test
 *
 *  Start the server first
 */

#include <iostream>
#include <stdexcept>
#include <vector>

#include <str/Convert.h>
#include <sys/Path.h>
#include <sys/LocalDateTime.h>
#include <mem/SharedPtr.h>
#include <net/ClientSocketFactory.h>

int main(int argc, char** argv)
{
    try
    {
        // Parse the command line
        if (argc != 5)
        {
            std::cerr << "Usage: " << sys::Path::basename(argv[0])
                      << " <host> <port> <buffer size MB> <MB to send>\n\n";
            return 1;
        }
        const std::string host(argv[1]);
        const int port = str::toType<int>(argv[2]);
        const size_t bufferSize = str::toType<size_t>(argv[3]) * 1024 * 1024;
        const size_t bytesToSend = str::toType<size_t>(argv[4]) * 1024 * 1024;

        net::SocketAddress sa(host, port);
        std::unique_ptr<net::Socket> socket =
                net::TCPClientSocketFactory().create(sa);

        std::vector<sys::ubyte> bufferVec(std::min(bufferSize, bytesToSend), 0);
        sys::ubyte* const buffer = &bufferVec[0];

        // First send the # of bytes we'll be sending
        sys::Uint64_T numBytesMsg(bytesToSend);
        socket->send(&numBytesMsg, sizeof(sys::Uint64_T));

        // Then send all the bytes
        const sys::LocalDateTime start;
        size_t numBytesSent(0);
        while (numBytesSent < bytesToSend)
        {
            const size_t numBytesThisTime =
                    std::min(bufferSize, bytesToSend - numBytesSent);
            socket->send(buffer, numBytesThisTime);
            numBytesSent += numBytesThisTime;
        }

        // Wait for an ack back
        sys::Uint64_T ack;
        socket->recv(&ack, sizeof(sys::Uint64_T));
        const sys::LocalDateTime stop;

        socket->close();

        const double numSec =
                (stop.getTimeInMillis() - start.getTimeInMillis()) / 1000;
        const double numMB = bytesToSend / (1024.0 * 1024);
        const double mbPerSec = numMB / numSec;
        std::cout << "Sent " << numMB << " MB in " << numSec << " sec ("
                  << mbPerSec << " MB / s)\n";

        return 0;
    }
    catch (const except::Exception& ex)
    {
        std::cerr << ex.toString() << std::endl;
        return 1;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception\n";
        return 1;
    }
}

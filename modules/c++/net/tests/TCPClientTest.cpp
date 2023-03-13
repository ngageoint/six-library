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
 *  \brief 2 transaction TCP client
 *
 *  Create a TCP socket and a connection to a server (using a 3-way handshake).
 *
 *  The packet we transfer is borrowed from the UDP client test, and has no
 *  real importance (especially with regard to TCP).
 *
 *  As we are a client to another machine, we need the IP address and port of
 *  a serving machine as arguments.
 *
 *  File simply sends a packet to the server, and recv()'s back a return value
 *  back from it.
 */

#include <import/net.h>
#include <import/except.h>
#include <import/sys.h>
#include "my_packet.h"

using namespace net;

int main(int argc, char** argv)
{

    if (argc != 3)
    {
        die_printf("%s <host> <port>\n", argv[0]);
    }

    try
    {
        std::string host = argv[1];
        int port = atoi(argv[2]);

        SocketAddress sa(host, port);
        std::unique_ptr<Socket> socket = TCPClientSocketFactory().create(sa);
        my_packet_t packet;
        packet.packet_no = 1;
        packet.time_stamp = time(NULL);
        socket->send(&packet, sizeof(my_packet_t));
        int rv;
        socket->recv(&rv, sizeof(int));
        std::cout << "Recv'd: " << rv << " back from socket" << std::endl;
        socket->close();
        return 0;
    }
    catch (except::Exception& ex)
    {
        std::cout << ex.getTrace() << std::endl;
    }
}

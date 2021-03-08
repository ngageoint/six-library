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
 *  \brief 2 transaction UDP server
 *
 *  Create a UDP socket, bound to the socket address.
 *
 *  The packet we transfer
 *  is essentially meaningless at this time -- just a very basic
 *  kind of packet that approximates what you might send in real life.
 *
 *  As we are a server to another machine, we need the port that we will
 *  be bound to.
 *
 *  File simply recv's a packet from the client, and sends back a return value
 *  (1).
 */

#include <import/net.h>
#include <import/sys.h>
#include <import/except.h>
#include <import/mem.h>
#include "my_packet.h"

using namespace net;

int main(int argc, char** argv)
{

    if (argc != 2)
    {
        die_printf("Usage: %s <port>\n", argv[0]);
    }
    try
    {
        int port = atoi(argv[1]);

        SocketAddress address(port);
        mem::auto_ptr<Socket> socket = UDPServerSocketFactory().create(address);

        my_packet_t packet;
        SocketAddress cliAddr(port);
        socket->recvFrom(cliAddr, (char*)&packet, sizeof(my_packet_t));
        std::cout << "(in packet: #" << packet.packet_no << ")" << std::endl;
        int one = 1;
        socket->sendTo(cliAddr, (const char*)&one, 4);
        socket->close();
    }
    catch (except::Exception& ex)
    {
        std::cout << ex.getTrace() << std::endl;
    }
}

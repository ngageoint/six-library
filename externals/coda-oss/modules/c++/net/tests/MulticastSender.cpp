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

//#include <sys/types.h>
//#include <netinet/in.h>
//linux/in.h>

#include <import/net.h>
#include <import/sys.h>
#include <import/io.h>
#include <import/mem.h>

using namespace net;
using namespace sys;
using namespace io;
using namespace except;

mem::auto_ptr<Socket> createSenderSocket(SocketAddress& address, int loopback = 1)
{
    mem::auto_ptr<Socket> s(new Socket(UDP_PROTO));

    int on = 1;
    s->setOption(SOL_SOCKET, SO_REUSEADDR, on);

    // Turn off loopback to this machine
    if (!loopback)
        s->setOption(IPPROTO_IP, IP_MULTICAST_LOOP, loopback);

    //    s.bind(address);
    return s;
}

struct Packet
{
    int number;
    char what[128];

};

#define ACK_CHANNEL 8647
#define ACK_HOST "127.0.0.1"
int main(int argc, char** argv)
{
    try
    {
        if (argc != 3)
            die_printf("Usage: %s <multicast-group> <port>\n");
        std::string mcastGroup = argv[1];
        int port = atoi(argv[2]);
        SocketAddress sa(mcastGroup, port);

        // Register ourselves with the OS as members of this group
        mem::auto_ptr<Socket> socket = createSenderSocket(sa);
        Packet packet;
        std::string myMessage = "Hello group!";
        memcpy(packet.what, myMessage.c_str(), myMessage.length());
        packet.what[myMessage.length()] = 0;
        packet.number = 1;
        socket->sendTo(sa, (const char*) &packet, sizeof(packet));

    }
    catch (Exception& ex)
    {
        std::cout << ex.toString() << std::endl;
    }

}


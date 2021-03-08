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

#include <import/net.h>
#include <import/sys.h>
#include <import/io.h>
#include <import/mem.h>

using namespace net;
using namespace sys;
using namespace io;
using namespace except;

mem::auto_ptr<Socket> createMulticastSubscriber(const std::string& group,
        const SocketAddress& local)
{
    mem::auto_ptr<Socket> socket(new Socket(UDP_PROTO));

    struct ip_mreq mreq;

    //const net::SockAddrIn_T& in = address.getAddress();

    // Need to initialize our structure properly here.
    //     memcpy(&mreq.imr_multiaddr,
    // 	   &in.sin_addr,
    // 	   sizeof(struct in_addr));

    // We want to let the kernel choose the interface device
    // (eth0,1, etc)
    std::cout << "Here!" << std::endl;
    mreq.imr_multiaddr.s_addr = inet_addr(group.c_str());
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    // Now set our socket option to add us as members
    socket->setOption(IPPROTO_IP, IP_ADD_MEMBERSHIP, mreq);

    socket->bind(local);

    return socket;
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

        SocketAddress here(port);

        // Register ourselves with the OS as members of this group

        mem::auto_ptr<Socket> socket = createMulticastSubscriber(mcastGroup, here);
        Packet packet;
        SocketAddress whereFrom;
        socket->recvFrom(whereFrom, (char*) &packet, sizeof(packet));
        std::cout << "Recv'd message: " << packet.what << std::endl;
        std::cout << "Packet #: " << packet.number << std::endl;
        socket->close();
    }
    catch (Exception& ex)
    {
        std::cout << ex.toString() << std::endl;
    }

}

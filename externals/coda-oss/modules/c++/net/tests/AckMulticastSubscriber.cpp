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
#include "net/SingleThreadedAllocStrategy.h"
#include "net/NetConnectionServer.h"

using namespace net;
using namespace sys;
using namespace io;
using namespace except;

template<typename T> class RetransmitTarget : public net::RequestHandler
{
public:

    RetransmitTarget()
    {
    }

    ~RetransmitTarget()
    {
    }
    void operator()(net::NetConnection* conn)
    {
        T packet;
        conn->read((char*) &packet, sizeof(packet));
        std::cout << "Recieved packet" << " on retransmit" << std::endl;
    }
};

template<typename T> class AckMulticastSubscriber
{
    mem::auto_ptr<Socket> mAckChannel;
    mem::auto_ptr<Socket> mMulticastSubscriber;
public:
    AckMulticastSubscriber(const std::string& mcastGroup, int mcastLocalPort,
            const std::string& replyTo, int replyToPort)
    {
        mMulticastSubscriber = createMulticastSubscriber(mcastGroup,
                                                         mcastLocalPort);
        mAckChannel = createSocketForAck(replyTo, replyToPort);
    }

    ~AckMulticastSubscriber()
    {
    }

    mem::auto_ptr<Socket> createMulticastSubscriber(const std::string& group, int port)
    {
        SocketAddress here(port);
        mem::auto_ptr<Socket> socket(new Socket(UDP_PROTO));
        socket->bind(here);
        struct ip_mreq mreq;

        mreq.imr_multiaddr.s_addr = inet_addr(group.c_str());
        // We want to let the kernel choose the interface device
        // (eth0,1, etc)
        mreq.imr_interface.s_addr = htonl(INADDR_ANY);

        // Now set our socket option to add us as members
        socket->setOption(IPPROTO_IP, IP_ADD_MEMBERSHIP, mreq);

        return socket;
    }

    mem::auto_ptr<Socket> createSocketForAck(const std::string& senderHost, int senderPort)
    {
        SocketAddress toSender(senderHost, senderPort);
        mem::auto_ptr<Socket> s = UDPClientSocketFactory().create(toSender);
        // This socket is already 'connect()'ed by now, so we use send()
        return s;
    }

    void waitForNotification(T& packet)
    {
        SocketAddress whereFrom;
        mMulticastSubscriber->recvFrom(whereFrom, (char*) &packet,
                                      sizeof(packet));
    }
    void confirmDelivery(int sequenceNumber)
    {
        mAckChannel->send((const char*) &sequenceNumber, sizeof(int));
    }
};

struct MyPacket
{
    int number;
    char what[128];
};

class RetransmitThread : public sys::Thread
{
    int mRetransmitPort;
public:
    RetransmitThread(int retransmitPort) :
        mRetransmitPort(retransmitPort)
    {
    }

    void run()
    {
        net::NetConnectionServer server;
        net::DefaultAllocStrategy* strategy = new net::DefaultAllocStrategy();
        strategy->setRequestHandlerFactory(new DefaultRequestHandlerFactory<
                RetransmitTarget<MyPacket>>());

        strategy->initialize();
        server.setAllocStrategy(strategy);
        server.create(mRetransmitPort);
        server.handleClients();

    }

};

int main(int argc, char** argv)
{
    try
    {
        if (argc != 6)
            die_printf(
                       "Usage: %s <multicast-group> <mulicast-recv-port> <reply-to> <at-port> <retransmit-port>\n",
                       argv[0]);
        std::string mcastGroup = argv[1];
        int mcastRecvPort = atoi(argv[2]);

        std::string replyTo = argv[3];
        int replyAt = atoi(argv[4]);
        int retransmitPort = atoi(argv[5]);

        sys::Thread* thr = new RetransmitThread(retransmitPort);
        thr->start();

        pthread_detach(thr->getNative());

        AckMulticastSubscriber<MyPacket>mcastSubs(mcastGroup, mcastRecvPort,
                                                  replyTo, replyAt);

        MyPacket packet;
        mcastSubs.waitForNotification(packet);

        std::cout << "Recv'd message: " << packet.what << std::endl;
        mcastSubs.confirmDelivery(packet.number);
        std::cout << "Confirmed packet #: " << packet.number << std::endl;

    }
    catch (Exception& ex)
    {
        std::cout << ex.toString() << std::endl;
    }

}

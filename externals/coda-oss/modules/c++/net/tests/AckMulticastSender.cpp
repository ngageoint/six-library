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

template<typename T> class AckMulticastSender
{

    std::unique_ptr<Socket> mMulticastSender;
    std::unique_ptr<Socket> mAckChannel;
    SocketAddress mMulticastAddr;
    std::vector<std::string> mSubscribers;
    int mRetransmitPort;
public:
    AckMulticastSender(const std::string& mcastGroup, int mcastPort,
            int localAckPort, const std::vector<std::string>& subscribers,
            int retransmitPort, int loopback = 1) :
        mRetransmitPort(retransmitPort)
    {
        mSubscribers = subscribers;
        mMulticastSender = createSenderSocket(mcastGroup, mcastPort, loopback);
        mAckChannel = createAckChannel(localAckPort);
    }

    ~AckMulticastSender()
    {
    }
    
    std::unique_ptr<Socket> createAckChannel(int localAckPort)
    {
        SocketAddress address(localAckPort);
        return UDPServerSocketFactory().create(address);
    }
    std::unique_ptr<Socket> createSenderSocket(const std::string& mcastGroup, int mcastPort,
            int loopback)
    {

        mMulticastAddr.set(mcastPort, mcastGroup);

        std::unique_ptr<Socket> s(new Socket(UDP_PROTO));

        int on = 1;
        s->setOption(SOL_SOCKET, SO_REUSEADDR, on);

        // Turn off loopback to this machine
        if (!loopback)
            s->setOption(IPPROTO_IP, IP_MULTICAST_LOOP, loopback);

        return s;
    }
    void sendNotification(const T& t)
    {
        mMulticastSender->sendTo(mMulticastAddr, (const char*) &t, sizeof(t));
    }

    void confirmOrRetransmit(int number, const T& t)
    {
        std::vector < std::string > rsvp;
        // We should set a timer here, and collectRSVPs.
        // When the timer is up, we should retransmit to non-rsvp hosts
        // Now we need to get our retransmit list...
        collectRSVPs(number, rsvp);
        std::cout << "Collected RSVPs" << std::endl;
        std::vector < std::string > needRetransmit;
        initRetransmitList(rsvp, needRetransmit);
        retransmit(needRetransmit, t);

    }

    void retransmit(const std::vector<std::string>& needRetransmit,
            const T& packet)
    {
        for (int i = 0; i < needRetransmit.size(); i++)
        {
            SocketAddress sa(needRetransmit[i], mRetransmitPort);
            std::unique_ptr<Socket> toRetransmit = net::TCPClientSocketFactory().create(sa);
            toRetransmit.send((const char*) &packet, sizeof(packet));
            toRetransmit.close();
        }

    }

    void initRetransmitList(const std::vector<std::string>& rsvped,
            std::vector<std::string>& retransmitList)
    {
        for (int i = 0; i < mSubscribers.size(); i++)
        {
            std::vector<std::string>::const_iterator it =
                    std::find(rsvped.begin(), rsvped.end(), mSubscribers[i]);
            if (it == rsvped.end())
            {
                retransmitList.push_back(mSubscribers[i]);
                std::cout << mSubscribers[i]
                        << " identified as needing retransmit" << std::endl;
            }
        }

    }

    void collectRSVPs(int number, std::vector<std::string>& rsvps)
    {

        for (int i = 0; i < mSubscribers.size(); i++)
        {
            SocketAddress whereFrom;
            int myNumber;

            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 0; // 1 millisecond
            //std::cout << (int) mAckChannel->getHandle() << std::endl;
            // Platform dependent code....
            fd_set readers;//, writers;
            FD_ZERO(&readers);//        FD_ZERO(&writers);
            FD_SET(mAckChannel->getHandle(), &readers);
            //FD_SET(mAckChannel->getHandle(), &writers);
            assert(FD_ISSET(mAckChannel->getHandle(), &readers));
            //int rv = 1;
            int rv = ::select(mAckChannel->getHandle() + 1, &readers, NULL,
                              NULL, &tv);
            if (rv < 0)
            {
                throw sys::SocketException(Ctxt("Select failed"));
            }
            // Platform dependent code....

            if (rv)
            {
                mAckChannel->recvFrom(whereFrom, (char*) &myNumber, sizeof(int));
                if (myNumber == number)
                {
                    std::string host =
                            inet_ntoa(whereFrom.getAddress().sin_addr);
                    std::cout << "Recv'd ack from: " << host
                            << " for packet #: " << number << std::endl;
                    rsvps.push_back(host);
                }
            }
            else
            {
                std::cout << "Timed out" << std::endl;
            }
        }
    }
};

struct MyPacket
{
    int number;
    char what[128];
};

int main(int argc, char** argv)
{
    try
    {
        if (argc < 6)
            die_printf(
                       "Usage: %s <multicast-group> <multicast-local-port> <our-ack-port> <retransmit-port> <subscriber1> ... <subscriberN>\n",
                       argv[0]);
        std::string mcastGroup = argv[1];
        int mcastPort = atoi(argv[2]);
        int ourAckPort = atoi(argv[3]);
        int retransmitPort = atoi(argv[4]);
        std::vector < std::string > subs;

        for (int i = 5; i < argc; i++)
            subs.push_back(argv[i]);

        for (int i = 0; i < subs.size(); i++)
            std::cout << subs[i] << std::endl;

        AckMulticastSender<MyPacket>multicastSender(mcastGroup, mcastPort,
                                                    ourAckPort, subs,
                                                    retransmitPort); // one listener for now

        MyPacket packet;
        std::string myMessage = "Hello group!";
        memcpy(packet.what, myMessage.c_str(), myMessage.length());
        packet.what[myMessage.length()] = 0;
        packet.number = 1;
        multicastSender.sendNotification(packet);

        multicastSender.confirmOrRetransmit(packet.number, packet);
    }
    catch (Exception& ex)
    {
        std::cout << ex.toString() << std::endl;
    }

}


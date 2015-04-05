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
#include <import/io.h>
#include <import/except.h>
#include <import/sys.h>

using namespace std;
using namespace net;
using namespace except;
using namespace io;
using namespace sys;

const static std::string SEND_THIS = "Hello, Server";

int main(int argc, char **argv)
{

    try
    {
        if (argc != 3)
            throw(Exception(FmtX("Usage: %s <host> <port>", argv[0])));

        std::string host(argv[1]);
        int port(atoi(argv[2]));

        net::SocketAddress addr(host, port);

        NetConnectionClientFactory clientBuilder;
        NetConnection* toServer = clientBuilder.create(addr);

        cout << "Sending: \"" << SEND_THIS << "\" to server" << endl;
        // Send a block


        unsigned int length = SEND_THIS.length();
        toServer->write((const char*) &length, 4);
        toServer->write(SEND_THIS.c_str(), length);

        // Recv a block
        char recvThis[1024];
        toServer->read((char*) &length, 4);
        toServer->read(recvThis, length);

        cout << "Received response: \"" << recvThis << "\" Back from server"
                << endl;
        clientBuilder.destroy(toServer);
    }
    catch (except::Throwable& t)
    {
        cout << t.toString() << endl;
        exit( EXIT_FAILURE);
    }
}

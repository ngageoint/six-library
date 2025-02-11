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
#include <assert.h>
#include <stdlib.h>
#include "net/SingleThreadedAllocStrategy.h"
#include "net/NetConnectionServer.h"

using namespace std;
using namespace io;
using namespace except;
using namespace sys;
using namespace net;

const static std::string SEND_THIS = "Hello, Client";

#define MAX_BUF_SIZE 1024

#define RET_STR " recv'd."

class EchoHandler : public net::RequestHandler
{
public:
    EchoHandler()
    {
    }
    ~EchoHandler()
    {
    }
    void operator()(net::NetConnection* conn)
    {
        char buf[MAX_BUF_SIZE];
        unsigned int length;
        conn->read((char*) &length, sizeof(unsigned int));
        assert(length <= (MAX_BUF_SIZE - strlen(RET_STR)));
        conn->read(buf, length);
        buf[length] = 0;
        std::cout << "Received: " << buf << std::endl;
        std::cout << "returning it to client..." << std::endl;
        memcpy(&buf[length], RET_STR, strlen(RET_STR));
        length = length + static_cast<unsigned int>(strlen(RET_STR));
        buf[length] = 0;
        conn->write((const char*) &length, 4);
        conn->write((const char*) buf, length);

    }
};

int main(int argc, char **argv)
{
    try
    {
        if (argc < 2)
            throw Exception(FmtX("Usage: %s <port> (-mt|-st|-tp)", argv[0]));

        net::AllocStrategy* strategy = NULL;

        if (argc == 3)
        {
            std::string arg(argv[2]);
            if (arg == "-mt")
                strategy = new net::PerRequestThreadAllocStrategy();
            else if (arg == "-tp")
                strategy = new net::ThreadPoolAllocStrategy(2);
            else if (arg == "-st")
                strategy = new net::SingleThreadedAllocStrategy();
        }
        net::NetConnectionServer server;
        server.initialize(new DefaultRequestHandlerFactory<EchoHandler>(),
                          strategy);
        server.create(atoi(argv[1]));
    }
    catch (except::Throwable& t)
    {
        cout << t.toString() << endl;
        exit( EXIT_FAILURE);
    }
}

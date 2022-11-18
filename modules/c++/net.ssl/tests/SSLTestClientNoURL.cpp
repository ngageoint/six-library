/* =========================================================================
 * This file is part of net.ssl-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * net.ssl-c++ is free software; you can redistribute it and/or modify
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
#include <import/net/ssl.h>
#include <import/io.h>
#include <import/except.h>
#include <import/sys.h>

using namespace std;
using namespace net;
using namespace except;
using namespace io;
using namespace sys;

const static std::string SEND_THIS = "Hello, Server";

int main(int argc, char** argv)
{
#if defined(USE_OPENSSL)
    try
    {
        if (argc != 3)
        throw Exception(FmtX("Usage: %s <host> <port>", argv[0]));

        std::string host(argv[1]);
        int port(atoi(argv[2]));

        cout << host << endl;
        cout << port << endl;

        cout << "Connecting to: " << host << ":" << port << endl;
        URL url(host);
        url.setPort(port);

        net::ssl::SSLConnectionClientFactory clientBuilder;
        NetConnection * toUrl = clientBuilder.create(url);

        cout << "Sending this to Url: " << SEND_THIS << endl;
        // Send a block
        sys::byte sendThis[128] = "Hello, Server";
        toUrl->write(sendThis, 128);

        // Recv a block
        sys::byte recvThis[128];
        memset(recvThis, 0, 128);
        toUrl->read(recvThis, 128);
        cout << "Received response: \"" << recvThis << "\" Back from server" << endl;

        clientBuilder.destroy(toUrl);
    }
    catch (except::Throwable& t)
    {
        cout << t.toString() << endl;
        exit(EXIT_FAILURE);
    }
#else
    (void)argc;
    (void)argv;
#endif
}


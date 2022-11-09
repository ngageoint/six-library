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
using namespace io;
using namespace except;
using namespace sys;

const static std::string SEND_THIS = "Hello, Server";

int main(int argc, char **argv)
{

    try
    {
        if (argc != 2)
            throw Exception(FmtX("Usage: %s <url>", argv[0]));

        URL url(argv[1]);

        cout << url.getHost() << endl;
        cout << url.getPort() << endl;
        cout << url.toString() << endl;
        cout << "Connecting to: " << url.toString() << endl;

        NetConnectionClientFactory clientBuilder(TCP_PROTO);
        NetConnection *toUrl = clientBuilder.create(url);

        SerializableConnection myConn(*toUrl);
        io::DataStream inData;
        io::DataStream outData;

        char buffer[20] = "HELLO WORLD!";

        outData.write((sys::byte*) buffer, sizeof(buffer));

        cout << "Sending this to Url: " << buffer << endl;
        // Send a block

        myConn.write(outData);
        myConn.read(inData);
        inData.read((sys::byte*) buffer, sizeof(buffer));
        cout << "Received response: \"" << buffer << "\" Back from server"
                << endl;

        clientBuilder.destroy(toUrl);
    }
    catch (except::Throwable& t)
    {
        cout << t.toString() << endl;
        exit( EXIT_FAILURE);
    }
}

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

#include <import/except.h>
#include <import/io.h>
#include <import/sys.h>
#include <import/net.h>

using namespace except;
using namespace io;
using namespace sys;
using namespace std;
using namespace net;

int main(int argc, char **argv)
{
    try
    {
        if (argc != 2)
            throw(Exception(FmtX("Usage: %s <url>", argv[0])));

        URL url(argv[1]);

        cout << "protocol: " << url.getProtocol() << endl;
        cout << "host: " << url.getHost() << endl;
        cout << "port: " << url.getPort() << endl;
        cout << "path: " << url.getPath() << endl;
        cout << "query: " << url.getQuery() << endl;
        cout << "fragment: " << url.getFragment() << endl;
        cout << "server: " << url.getServer() << endl;
        cout << "Url: " << url.toString() << endl;
    }
    catch (Throwable& t)
    {
        cout << "Caught Throwable: " << t.toString() << endl;
    }
    return 0;
}

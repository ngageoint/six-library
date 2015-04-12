/* =========================================================================
 * This file is part of io-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * io-c++ is free software; you can redistribute it and/or modify
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

#include <iostream>
#include <import/except.h>
#include <import/io.h>

using namespace io;
using namespace except;
using namespace std;

int main(int argc, char *argv[])
{
    try
    {

        std::string input("abcdefghijklmnopqrstuvwxyz");
        char buffer[32];
        memset(buffer, 0, 32);
        StringStream sStream;

        sStream.write((const sys::byte*) input.c_str(), 5);
        sStream.read((sys::byte*) buffer, 3);
        cout << buffer << endl;

        memset(buffer, 0, 32);
        sStream.write((const sys::byte*) input.c_str(), 10);
        sStream.read((sys::byte*) buffer, 11);
        cout << buffer << endl;

        memset(buffer, 0, 32);
        sStream.read((sys::byte*) buffer, 100);
        cout << buffer << endl;

    }
    catch (Throwable& t)
    {
        cerr << "Caught throwable: " << t.toString() << endl;
    }
    catch (...)
    {
        cerr << "Caught unknown exception" << endl;
    }
    return 0;
}

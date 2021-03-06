/* =========================================================================
 * This file is part of io-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
using namespace std;

#include <import/io.h>
using namespace io;

#include <import/sys.h>
using namespace sys;



int main(int, char**)
{
    std::string input("abcdefghijklmnopqrstuvwxyz");
    char buffer[32];
    memset(buffer, 0, 32);

    ByteStream bStream;
    bStream.write((const sys::byte*)input.c_str(), 5);
    bStream.seek(0, Seekable::START);
    bStream.read((sys::byte*)buffer, 3);
    cout << "1:" << buffer << endl;

    memset(buffer, 0, 32);
    bStream.seek(5, Seekable::START);
    bStream.write((const sys::byte*)input.c_str(), 10);
    bStream.seek(3, Seekable::START);
    bStream.read((sys::byte*)buffer, 11);
    cout << "2:" << buffer << endl;

    memset(buffer, 0, 32);
    bStream.read((sys::byte*)buffer, 100);
    cout << "3:" << buffer << endl;


    return 0;
}

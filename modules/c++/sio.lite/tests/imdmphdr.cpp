/* =========================================================================
 * This file is part of sio.lite-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * sio.lite-c++ is free software; you can redistribute it and/or modify
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
#include <import/str.h>
#include <import/sio/lite.h>

using namespace sio::lite;
using namespace io;
using namespace except;


std::string printPrintable(std::vector<sys::byte>& bytes)
{
    std::ostringstream printable;
    for (unsigned int i = 0; i < bytes.size(); i++)
    {
        int x = (int)bytes[i];
        if (isgraph(x) || isspace(x))
            printable << (char)x;
        else
            printable << "*";
    }
    return printable.str();
}


void printUserData(FileHeader* h)
{
    sio::lite::UserDataDictionary& userData = h->getUserDataSection();
    for (sio::lite::UserDataDictionary::Iterator it = userData.begin();
        it != userData.end(); ++it)
    {
        std::cout << ". " << it->first << ": ";
        std::cout << "[" << printPrintable( it->second ) << "]" << std::endl;
    }
}

int main(int argc, char** argv)
{
    if (argc != 2)
        die_printf("Usage: %s <sio-file>\n", argv[0]);
    try
    {
        //  Load a CODA stream
        FileInputStream input(argv[1]);
        //  Create a reader
        FileReader r(&input);
        FileHeader* header = r.readHeader();

        // Now let's spit out header info
        std::cout << ". nl: " << header->getNumLines() << std::endl;
        std::cout << ". ne: " << header->getNumElements() << std::endl;
        std::cout << ". et: " << header->getElementTypeAsString() << std::endl;
        std::cout << ". es: " << header->getElementSize() << std::endl;

        printUserData(header);
    }
    catch (Exception& e)
    {
        std::cout << "Caught exception: " << e.getMessage() << std::endl;
        std::cout << "Trace:" << std::endl << e.getTrace() << std::endl;
    }
}


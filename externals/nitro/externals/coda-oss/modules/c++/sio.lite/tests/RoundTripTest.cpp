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
#include <import/sio/lite.h>
#include <complex>

using namespace sio::lite;
using namespace io;
using namespace except;

void printFloat(float* f)
{
    char* buf = (char*)f;
    printf("Val: %x,%x,%x,%x\n", buf[0], buf[1], buf[2], buf[3]);
}

int main(int argc, char** argv)
{
    if (argc < 2 || argc > 3)
        die_printf("Usage: %s <sio-file> (<sio-output>)\n", argv[0]);

    try
    {
        //  Load a CODA stream
        FileInputStream input(argv[1]);

        //  Create a reader
        FileReader r(&input);
        FileHeader* header = r.readHeader();
        
        int len = header->getNumElements() * header->getNumLines();
        int elemSize = header->getElementSize();
        sys::byte* buf = new sys::byte[len*elemSize];
        r.read(buf, len*elemSize);
        
        std::cout << "Different byte order? "
            << (header->isDifferentByteOrdering() ? "yes" : "no") << std::endl;
        std::cout << "Input Header length: " << header->getLength() << std::endl;
        std::cout << "Input Data length: " << (len * elemSize) << std::endl;
        
        std::string outputFile = ((argc == 3) ? (argv[2]) : (std::string(argv[1]) + ".out"));
        FileWriter writer(outputFile);
        writer.write(header, buf, 1);
        delete [] buf;
        
        //read the new one!
        FileInputStream input2(outputFile);
        r.setInputStream(&input2);
        header = r.readHeader();
        
        len = header->getNumElements() * header->getNumLines();
        elemSize = header->getElementSize();
        std::cout << "Output Header length: " << header->getLength() << std::endl;
        std::cout << "Output Data length: " << (len * elemSize) << std::endl;
    }
    catch (Exception& e)
    {
        std::cout << "Caught exception: " << e.getMessage() << std::endl;
        std::cout << "Trace:" << std::endl << e.getTrace() << std::endl;
    }
    return 0;
}



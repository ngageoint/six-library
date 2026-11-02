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

using namespace sio::lite;
using namespace io;
using namespace except;


int main(int argc, char** argv)
{
    //TODO in the future, take in other inputs, such as rows, cols, size, type, etc.
    if (argc != 2)
        die_printf("Usage: %s <sio-output>\n", argv[0]);

    int width = 2;
    int height = 2;
    int elemSize = 4;
    int type = FileHeader::FLOAT;
    int numBands = 1;

    try
    {
        size_t bufSize = width * height * elemSize;
        float** rawData = new float*[numBands];
        for (int i = 0; i < numBands; ++i)
            rawData[i] = new float[bufSize];

        float base = 6.73457834987f;

        //fill with trash data
        for (int i = 0; i < numBands; ++i)
            for (size_t j = 0; j < bufSize; ++j)
                rawData[i][j] = base + j;

        std::string outputFile = argv[1];
        FileWriter writer(outputFile);
        FileHeader hdr(height, width, elemSize, type);

        //add some test data to the header
        std::vector<sys::byte> udEntry;
        std::string uData = "ABCABCABCABC";
        for (size_t i = 0; i < uData.size(); i++)
            udEntry.push_back((sys::byte)uData[i]);
        
        hdr.addUserData("junk", udEntry);
        hdr.addUserData("name", "Tom Zellman");
        hdr.addUserData("int_12345", 12345);

        writer.write(&hdr, (const sys::byte*)rawData, numBands);
        for (int ii = 0; ii < numBands; ++ii)
        {
            delete[] rawData[ii];
        }
        delete[] rawData;
        return 0;
    }
    catch (const Exception& e)
    {
        std::cerr << "Caught exception: " << e.getMessage() << std::endl;
        std::cerr << "Trace:" << std::endl << e.getTrace() << std::endl;
    }
    return 1;
}

/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * six-c++ is free software; you can redistribute it and/or modify
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
#include <import/six.h>
#include <import/six/sicd.h>

using namespace six;

/*!
 *  We need to read in a NITF SICD file and dump the data as a raw
 *  file to an output file.  This output can be compared against
 *  NITRO's test_dump_record by cat-ing the results of each segment from
 *  test_dump_record together and diffing against this file.
 */

int main(int argc, char** argv)
{

    if (argc != 2)
    {
        die_printf("Usage: %s <sicd-image-file>\n", argv[0]);
    }
    std::string inputFile(argv[1]);
    UByte* workBuffer = NULL;

    try
    {
        XMLControlFactory::getInstance().
            addCreator(
                six::DATA_COMPLEX, 
                new XMLControlCreatorT<six::sicd::ComplexXMLCreator>()
                );
        ReadControl* reader = new NITFReadControl();

        reader->load(inputFile);

        Container *container = reader->getContainer();

        std::string base = sys::Path::basename(inputFile, true);

        Region region;
        region.setNumRows(1);

        Data* sicdData = container->getData(0);
        unsigned long nbpp = sicdData->getNumBytesPerPixel();
        unsigned long height = sicdData->getNumRows();
        unsigned long width = sicdData->getNumCols();
        unsigned long nbpr = nbpp * width;
        workBuffer = new UByte[nbpr];
        region.setBuffer(workBuffer);

        std::string outputFile = FmtX("%s-%dx%d_%d.raw", base.c_str(), height,
                width, nbpp);
        io::FileOutputStream outputStream(outputFile);

        // If you dont set the region, we assume that it will span the
        // entire image


        for (unsigned int i = 0; i < height; i++)
        {
            region.setStartRow(i);
            UByte* line = reader->interleaved(region, 0);
            outputStream.write((const sys::byte*) line, nbpr);

        }
        delete[] workBuffer;
        delete reader;
        workBuffer = NULL;
        outputStream.close();
        return 0;
    }
    catch (except::Exception& e)
    {
        std::cout << e.getMessage() << std::endl;
        if (workBuffer)
            delete[] workBuffer;
    }
}

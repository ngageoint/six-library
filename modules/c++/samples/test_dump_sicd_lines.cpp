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

#ifdef USE_SIO_LITE
#  include <import/sio/lite.h>
#endif

using namespace six;

/*!
 *  We need to read in a NITF SICD file and dump the data as a raw
 *  file to an output file.  This output can be compared against
 *  test_dump_record in by cat-ing the results of each segment from
 *  TDR together and diffing against this file.
 */

const char* USAGE =
        "Usage: %s <sicd-image-file> (-sr <d>) (-nr <d>) (-sc <d>) (-nc <d>) (-sio)\n";

/*!
 *  If this is going to be an SIO file, and there is an SIO file writer,
 *  use it.  Otherwise, this function will be empty.  That way its less
 *  intrusive to the example
 *
 */
void writeSIOHeader(long numRows, long numCols, io::OutputStream& outputStream)
{
#ifdef USE_SIO_LITE
    // Write out the SIO header first
    sio::lite::FileHeader fileHeader(numRows, numCols, 4,
            sio::lite::FileHeader::COMPLEX_FLOAT);

    fileHeader.to(1, outputStream);
#endif
}

int main(int argc, char** argv)
{
    long startRow = 0;
    long startCol = 0;
    long numRows = -1;
    long numCols = -1;
    bool isSIO = false;

    std::string inputFile;
    if (argc < 2)
    {
        die_printf(USAGE, argv[0]);
    }
    if (argc == 2)
        inputFile = argv[1];
    else
    {
        for (int i = 1; i < argc; i++)
        {
            std::string arg(argv[i]);
            if (arg == "-sr")
            {
                startRow = str::toType<long>(argv[++i]);
            }
            else if (arg == "-nr")
            {
                numRows = str::toType<long>(argv[++i]);
            }
            else if (arg == "-sc")
            {
                startCol = str::toType<long>(argv[++i]);
            }
            else if (arg == "-nc")
            {
                numCols = str::toType<long>(argv[++i]);
            }
#ifdef USE_SIO_LITE
            else if (arg == "-sio")
            {
                isSIO = true;
            }
#endif
            else
            {
                inputFile = argv[i];
            }
        }
        if (inputFile.empty())
            die_printf(USAGE, argv[0]);
    }
    UByte* workBuffer = NULL;

    try
    {

        XMLControlFactory::getInstance(). addCreator(
                                                     DataClass::COMPLEX,
                                                     new XMLControlCreatorT<
                                                             six::sicd::ComplexXMLControl>());
        NITFReadControl* reader = new NITFReadControl();
        reader->load(inputFile);

        Container* container = reader->getContainer();
        std::string base = sys::Path::basename(inputFile, true);

        // In this version, I will allocate the work buffer since I know
        // exactly how big it will be and we are reading lines, so I dont
        // want a new one allocated every time I read
        // case
        Region region;
        region.setStartRow(startRow);
        region.setStartCol(startCol);
        region.setNumCols(numCols);
        region.setNumRows(1);

        Data* sicdData = container->getData(0);
        unsigned long nbpp = sicdData->getNumBytesPerPixel();
        unsigned long height = sicdData->getNumRows();
        unsigned long width = sicdData->getNumCols();

        // This is just to show off that this function is available
        unsigned long nbpr = nbpp * width;

        workBuffer = new UByte[nbpr];
        region.setBuffer(workBuffer);

        if (numRows == -1)
            numRows = height;

        if (numCols == -1)
            numCols = width;

        std::string outputFile = FmtX("%s_%d-%dx%d-%d_%d.%s", base.c_str(),
                                      startRow, startRow + numRows, startCol,
                                      startCol + numCols, nbpp, isSIO ? "sio"
                                                                      : "raw");

        io::FileOutputStream outputStream(outputFile);

        if (isSIO)
        {
            writeSIOHeader(numRows, numCols, outputStream);
        }

        for (unsigned int i = startRow; i < numRows + startRow; i++)
        {
            region.setStartRow(i);
            UByte* line = reader->interleaved(region, 0);
            outputStream.write((const sys::byte*) line, nbpr);

        }

        std::string sicdXML = FmtX("%s.xml", base.c_str());
        io::FileOutputStream xmlStream(sicdXML);

        std::string xmlData = six::toXMLString(sicdData);
        xmlStream.write(xmlData.c_str(), xmlData.length());

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

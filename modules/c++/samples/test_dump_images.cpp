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
#include <mem/ScopedArray.h>
#include <import/cli.h>
#include <import/six.h>
#include <import/six/sicd.h>
#include <import/six/sidd.h>
#include <import/sio/lite.h>
#include "utils.h"

using namespace six;

void writeSIOFileHeader(long numRows,
                        long numCols,
                        six::PixelType pixelType,
                        io::OutputStream& outputStream)
{
    // Get the element type
    int elementSize;
    int elementType;
    switch (pixelType)
    {
    case six::PixelType::RE32F_IM32F:
        elementSize = sizeof(std::complex<float>);
        elementType = sio::lite::FileHeader::COMPLEX_FLOAT;
        break;
    case six::PixelType::RE16I_IM16I:
        elementSize = sizeof(std::complex<sys::Int16_T>);
        elementType = sio::lite::FileHeader::COMPLEX_SIGNED;
        break;
    case six::PixelType::MONO8I:
        elementSize = sizeof(sys::Int8_T);
        elementType = sio::lite::FileHeader::SIGNED;
        break;
    case six::PixelType::MONO16I:
        elementSize = sizeof(sys::Int16_T);
        elementType = sio::lite::FileHeader::SIGNED;
        break;
    default:
        throw except::Exception(Ctxt("Unexpected pixel type " +
                                         str::toString(pixelType)));
    }

    // Write out the SIO header first
    sio::lite::FileHeader(numRows,
                          numCols,
                          elementSize,
                          elementType).to(1, outputStream);
}

int main(int argc, char** argv)
{

    try
    {
        // create a parser and add our options to it
        cli::ArgumentParser parser;
        parser.setDescription(
                              "This program reads in a single SICD or SIDD file and dumps its images as separate files.");
        parser.addArgument("-d --dir", "Write to output directory", cli::STORE)->setDefault(
                                                                                            ".");
        parser.addArgument("--sr", "Start Row", cli::STORE, "startRow", "ROW")->setDefault(
                                                                                           0);
        parser.addArgument("--nr", "Number of Rows", cli::STORE, "numRows",
                           "ROWS")->setDefault(-1);
        parser.addArgument("--sc", "Start Col", cli::STORE, "startCol", "COL")->setDefault(
                                                                                           0);
        parser.addArgument("--nc", "Number of Cols", cli::STORE, "numCols",
                           "COLS")->setDefault(-1);
        parser.addArgument("--sio", "Write out an SIO instead of a RAW file",
                           cli::STORE_TRUE, "sio");
        parser.addArgument("-s --schema",
                           "Specify a schema or directory of schemas",
                           cli::STORE);
        parser.addArgument("--one --single",
                           "Read input image in one read, rather than lines",
                           cli::STORE_TRUE, "oneRead");
        parser.addArgument("file", "SICD/SIDD input file", cli::STORE, "file",
                           "FILE", 1, 1);

        const std::auto_ptr<cli::Results>
            options(parser.parse(argc, (const char**) argv));

        long startRow(options->get<long> ("startRow"));
        long numRows(options->get<long> ("numRows"));
        long startCol(options->get<long> ("startCol"));
        long numCols(options->get<long> ("numCols"));
        bool isSIO(options->get<bool> ("sio"));
        bool oneRead(options->get<bool> ("oneRead"));
        std::string inputFile(options->get<std::string> ("file"));
        std::string outputDir(options->get<std::string> ("dir"));
        std::vector<std::string> schemaPaths;
        getSchemaPaths(*options, "--schema", "schema", schemaPaths);

        // create an XML registry
        // The reason to do this is to avoid adding XMLControlCreators to the
        // XMLControlFactory singleton - this way has more fine-grained control
        XMLControlRegistry xmlRegistry;
        xmlRegistry.addCreator(DataType::COMPLEX, new XMLControlCreatorT<
                six::sicd::ComplexXMLControl> ());
        xmlRegistry.addCreator(DataType::DERIVED, new XMLControlCreatorT<
                six::sidd::DerivedXMLControl> ());

        // create a Reader registry (now, only NITF and TIFF)
        ReadControlRegistry readerRegistry;
        readerRegistry.addCreator(new NITFReadControlCreator());
        readerRegistry.addCreator(new six::sidd::GeoTIFFReadControlCreator());

        // get the correct ReadControl for the given file
        const std::auto_ptr<ReadControl>
            reader(readerRegistry.newReadControl(inputFile));
        // set the optional registry, since we have one
        reader->setXMLControlRegistry(&xmlRegistry);

        // load the file
        reader->load(inputFile, schemaPaths);

        Container* container = reader->getContainer();
        std::string base = sys::Path::basename(inputFile, true);
        size_t numImages = 0;

        if (container->getDataType() == DataType::COMPLEX
                && container->getNumData() > 0)
        {
            numImages = 1;
        }
        else if (container->getDataType() == DataType::DERIVED)
        {
            for (; numImages < container->getNumData()
                    && container->getData(numImages)->getDataType()
                            == DataType::DERIVED; ++numImages)
                ;
        }

        std::cout << "Found: " << numImages << " image(s)" << std::endl;

        sys::OS os;
        if (!os.exists(outputDir))
            os.makeDirectory(outputDir);

        // first, write out the XMLs
        for (size_t i = 0, total = container->getNumData(); i < total; ++i)
        {
            const Data* data = container->getData(i);
            std::string filename = FmtX("%s_DES_%d.xml", base.c_str(), i);
            std::string xmlFile = sys::Path::joinPaths(outputDir, filename);
            io::FileOutputStream xmlStream(xmlFile);

            std::string xmlData = six::toXMLString(data, &xmlRegistry);
            xmlStream.write(xmlData.c_str(), xmlData.length());
            xmlStream.close();
        }

        //  now, dump the images
        for (size_t ii = 0; ii < numImages; ++ii)
        {
            const Data* const data = container->getData(ii);
            const size_t nbpp = data->getNumBytesPerPixel();
            const size_t height = data->getNumRows();
            const size_t width = data->getNumCols();

            if (numRows == -1)
                numRows = height;

            if (numCols == -1)
                numCols = width;

            const std::string filename =
                FmtX("%s_%d-%dx%d-%d_%d-image-%d.%s",
                     base.c_str(),
                     startRow,
                     startRow + numRows,
                     startCol,
                     startCol + numCols,
                     nbpp,
                     ii,
                     isSIO ? "sio" : "raw");

            const std::string outputFile =
                sys::Path::joinPaths(outputDir, filename);
            io::FileOutputStream outputStream(outputFile);

            if (isSIO)
            {
                writeSIOFileHeader(numRows,
                                   numCols,
                                   data->getPixelType(),
                                   outputStream);
            }

            Region region;
            region.setStartRow(startRow);
            region.setStartCol(startCol);
            region.setNumCols(numCols);

            if (oneRead)
            {
                region.setNumRows(numRows);
                size_t totalBytes = nbpp * numCols * numRows;
                const mem::ScopedArray<UByte>
                    workBuffer(new UByte[totalBytes]);
                region.setBuffer(workBuffer.get());

                reader->interleaved(region, ii);
                outputStream.write((const sys::byte*) workBuffer.get(),
                                   totalBytes);
            }
            else
            {
                region.setNumRows(1);
                size_t nbpr = nbpp * width;

                // allocate this so we can reuse it for each row
                const mem::ScopedArray<UByte> workBuffer(new UByte[nbpr]);
                region.setBuffer(workBuffer.get());

                for (unsigned int jj = startRow;
                     jj < numRows + startRow;
                     ++jj)
                {
                    region.setStartRow(jj);
                    UByte* line = reader->interleaved(region, ii);
                    outputStream.write((const sys::byte*) line, nbpr);
                }
            }
            outputStream.close();
            std::cout << "Wrote file: " << outputFile << std::endl;
        }
    }
    catch (const except::Exception& e)
    {
        std::cout << e.toString() << std::endl;
        exit(1);
    }
    catch (const std::exception& cppE)
    {
        std::cout << "C++ exception: " << cppE.what() << std::endl;
        exit(1);
    }
    catch (...)
    {
        std::cout << "Unknown exception" << std::endl;
        exit(1);
    }
    return 0;
}

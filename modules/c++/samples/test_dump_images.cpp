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
#include <sys/ScopedArray.h>
#include <import/cli.h>
#include <import/six.h>
#include <import/six/sicd.h>
#include <import/six/sidd.h>
#include <import/sio/lite.h>

using namespace six;

void writeSIOFileHeader(long numRows, long numCols, unsigned long nbpp,
                        io::OutputStream& outputStream)
{
    int type = nbpp == 1 ? 1 : 22;
    // Write out the SIO header first
    sio::lite::FileHeader fileHeader(numRows, numCols, nbpp, type);

    fileHeader.to(1, outputStream);
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
        reader->load(inputFile);

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
        for (size_t i = 0; i < numImages; ++i)
        {
            const Data* data = container->getData(i);
            unsigned long nbpp = data->getNumBytesPerPixel();
            unsigned long height = data->getNumRows();
            unsigned long width = data->getNumCols();

            if (numRows == -1)
                numRows = height;

            if (numCols == -1)
                numCols = width;

            std::string filename = FmtX("%s_%d-%dx%d-%d_%d-image-%d.%s",
                                        base.c_str(), startRow, startRow
                                                + numRows, startCol, startCol
                                                + numCols, nbpp, i,
                                        isSIO ? "sio" : "raw");

            std::string outputFile = sys::Path::joinPaths(outputDir, filename);
            io::FileOutputStream outputStream(outputFile);

            if (isSIO)
            {
                writeSIOFileHeader(numRows, numCols, nbpp, outputStream);
            }

            Region region;
            region.setStartRow(startRow);
            region.setStartCol(startCol);
            region.setNumCols(numCols);

            if (oneRead)
            {
                region.setNumRows(numRows);
                unsigned long totalBytes = nbpp * width * height;
                const sys::ScopedArray<UByte>
                    workBuffer(new UByte[totalBytes]);
                region.setBuffer(workBuffer.get());

                reader->interleaved(region, i);
                outputStream.write((const sys::byte*) workBuffer.get(),
                                   totalBytes);
            }
            else
            {
                region.setNumRows(1);
                unsigned long nbpr = nbpp * width;

                // allocate this so we can reuse it for each row
                const sys::ScopedArray<UByte> workBuffer(new UByte[nbpr]);
                region.setBuffer(workBuffer.get());

                for (unsigned int j = startRow; j < numRows + startRow; j++)
                {
                    region.setStartRow(j);
                    UByte* line = reader->interleaved(region, i);
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

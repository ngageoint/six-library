/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#include <sstream>
#include <limits>

#include <std/filesystem>

#include <import/six.h>

#include <mem/ScopedArray.h>
#include <import/cli.h>
#include <import/six/sicd.h>
#include <import/six/sidd.h>
#include <import/sio/lite.h>
#include "utils.h"

namespace fs = std::filesystem;

namespace
{
void writeSIOFileHeader(size_t numRows,
                        size_t numCols,
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
        elementSize = sizeof(types::complex<int16_t>);
        elementType = sio::lite::FileHeader::COMPLEX_SIGNED;
        break;
    case six::PixelType::MONO8I:
        elementSize = sizeof(int8_t);
        elementType = sio::lite::FileHeader::SIGNED;
        break;
    case six::PixelType::MONO16I:
        elementSize = sizeof(int16_t);
        elementType = sio::lite::FileHeader::SIGNED;
        break;
    default:
        throw except::Exception(Ctxt("Unexpected pixel type " +
                                         str::toString(pixelType)));
    }

    // Write out the SIO header first
    sio::lite::FileHeader(static_cast<int>(numRows),
                          static_cast<int>(numCols),
                          elementSize,
                          elementType).to(1, outputStream);
}
}

int main(int argc, char** argv)
{
    try
    {
        // create a parser and add our options to it
        cli::ArgumentParser parser;
        parser.setDescription(
                              "This program reads in a single SICD or SIDD file and dumps its images as separate files.");
        parser.addArgument("-d --dir", "Write to output directory", cli::STORE)->setDefault(".");
        parser.addArgument("--sr", "Start Row", cli::STORE, "startRow", "ROW")->setDefault(0);
        parser.addArgument("--nr", "Number of Rows", cli::STORE, "numRows",
                           "ROWS");
        parser.addArgument("--sc", "Start Col", cli::STORE, "startCol", "COL")->setDefault(0);
        parser.addArgument("--nc", "Number of Cols", cli::STORE, "numCols",
                           "COLS");
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

        const std::unique_ptr<cli::Results>
            options(parser.parse(argc, argv));

        size_t startRow(options->get<size_t>("startRow"));
        size_t startCol(options->get<size_t>("startCol"));
        const bool isSIO(options->get<bool>("sio"));
        const bool oneRead(options->get<bool>("oneRead"));
        const std::string inputFile(options->get<std::string>("file"));
        const std::string outputDir(options->get<std::string>("dir"));
        std::vector<std::string> schemaPaths;
        getSchemaPaths(*options, "--schema", "schema", schemaPaths);

        // create an XML registry
        // The reason to do this is to avoid adding XMLControlCreators to the
        // XMLControlFactory singleton - this way has more fine-grained control
        six::XMLControlRegistry xmlRegistry;
        xmlRegistry.addCreator<six::sicd::ComplexXMLControl>();
        xmlRegistry.addCreator<six::sidd::DerivedXMLControl>();

        // create a Reader registry (now, only NITF and TIFF)
        six::ReadControlRegistry readerRegistry;
        readerRegistry.addCreator(new six::NITFReadControlCreator());
        readerRegistry.addCreator(new six::sidd::GeoTIFFReadControlCreator());

        // get the correct ReadControl for the given file
        auto reader(readerRegistry.newReadControl(inputFile));
        // set the optional registry, since we have one
        reader->setXMLControlRegistry(&xmlRegistry);

        // load the file
        reader->load(inputFile, schemaPaths);

        auto container = reader->getContainer();
        const auto base = fs::path(inputFile).stem();
        size_t numImages = 0;

        if (container->getDataType() == six::DataType::COMPLEX
                && (!container->empty()))
        {
            numImages = 1;
        }
        else if (container->getDataType() == six::DataType::DERIVED)
        {
            for (; numImages < container->size()
                    && container->getData(numImages)->getDataType()
                            == six::DataType::DERIVED; ++numImages)
                ;
        }

        std::cout << "Found: " << numImages << " image(s)" << std::endl;

        if (!fs::exists(outputDir))
            fs::create_directory(outputDir);

        // first, write out the XMLs
        for (size_t i = 0, total = container->size(); i < total; ++i)
        {
            const six::Data* data = container->getData(i);
            std::string filename = FmtX("%s_DES_%d.xml", base.c_str(), i);
            const auto xmlFile = fs::path(outputDir) / filename;
            io::FileOutputStream xmlStream(xmlFile.string());

            const auto xmlData = six::toXMLString(data, &xmlRegistry);
            xmlStream.write(xmlData);
            xmlStream.close();
        }

        //  now, dump the images
        for (size_t ii = 0; ii < numImages; ++ii)
        {
            const six::Data* const data = container->getData(ii);
            const size_t nbpp = data->getNumBytesPerPixel();

            const size_t numRows = options->hasValue("numRows") ?
                    options->get<size_t>("numRows") : data->getNumRows();

            const size_t numCols = options->hasValue("numCols") ?
                    options->get<size_t>("numCols") : data->getNumCols();

            std::ostringstream filename;
            filename << base << "_"
                     << startRow << "-"
                     << (startRow + numRows) << "x"
                     << startCol << "-"
                     << (startCol + numCols) << "_"
                     << nbpp << "-image-"
                     << ii
                     << (isSIO ? "sio" : "raw");

            const auto outputFile = fs::path(outputDir) / filename.str();
            io::FileOutputStream outputStream(outputFile.string());

            if (isSIO)
            {
                writeSIOFileHeader(numRows,
                                   numCols,
                                   data->getPixelType(),
                                   outputStream);
            }

            six::Region region;
            region.setStartRow(startRow);
            region.setStartCol(startCol);
            region.setNumCols(numCols);

            if (oneRead)
            {
                region.setNumRows(numRows);
                size_t totalBytes = nbpp * numCols * numRows;
                const auto workBuffer = region.setBuffer(totalBytes);

                reader->interleaved(region, ii);
                outputStream.write((const std::byte*) workBuffer.get(),
                                   totalBytes);
            }
            else
            {
                region.setNumRows(1);
                const size_t nbpr = nbpp * numCols;

                // allocate this so we can reuse it for each row
                const auto workBuffer = region.setBuffer(nbpr);

                for (size_t jj = startRow;
                     jj < numRows + startRow;
                     ++jj)
                {
                    region.setStartRow(jj);
                    auto line = reader->interleaved(region, ii);
                    outputStream.write(line, nbpr);
                }
            }
            outputStream.close();
            std::cout << "Wrote file: " << outputFile << std::endl;
        }

        return 0;
    }
    catch (const except::Exception& e)
    {
        std::cerr << e.toString() << std::endl;
        return 1;
    }
    catch (const std::exception& cppE)
    {
        std::cerr << "C++ exception: " << cppE.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception" << std::endl;
        return 1;
    }
}

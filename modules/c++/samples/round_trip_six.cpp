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

#include <import/cli.h>
#include <import/io.h>
#include <import/mem.h>
#include <import/six.h>
#include <import/six/sicd.h>
#include <import/six/sidd.h>
#include "utils.h"

typedef std::complex<float> ComplexFloat;

int main(int argc, char** argv)
{
    try
    {
        // create a parser and add our options to it
        cli::ArgumentParser parser;
        parser.setDescription("This program reads a SICD or SIDD into the "\
                              "internal memory model and round-trips it back "\
                              "to file.");
        parser.addArgument("-e --expand", "Expand RE16I_IM16I to RE32F_IM32F",
                           cli::STORE_TRUE, "expand");
        parser.addArgument("-f --log", "Specify a log file", cli::STORE, "log",
                           "FILE")->setDefault("console");
        parser.addArgument("-l --level", "Specify log level", cli::STORE,
                           "level", "LEVEL")->setChoices(
                           str::split("debug info warn error"))->setDefault(
                           "info");
        parser.addArgument("-s --schema", 
                           "Specify a schema or directory of schemas",
                           cli::STORE);
        parser.addArgument("input", "Input SICD/SIDD", cli::STORE, "input",
                           "INPUT", 1, 1);
        parser.addArgument("output", "Output filename", cli::STORE, "output",
                           "OUTPUT", 1, 1);

        const std::auto_ptr<cli::Results>
            options(parser.parse(argc, (const char**) argv));

        const std::string inputFile(options->get<std::string> ("input"));
        const std::string outputFile(options->get<std::string> ("output"));
        const bool expand(options->get<bool> ("expand"));
        const std::string logFile(options->get<std::string> ("log"));
        std::string level(options->get<std::string> ("level"));
        std::vector<std::string> schemaPaths;
        getSchemaPaths(*options, "--schema", "schema", schemaPaths);

        str::upper(level);
        str::trim(level);
        logging::LogLevel logLevel(level);

        // create an XML registry
        // The reason to do this is to avoid adding XMLControlCreators to the
        // XMLControlFactory singleton - this way has more fine-grained control
        six::XMLControlRegistry xmlRegistry;
        xmlRegistry.addCreator(six::DataType::COMPLEX,
                               new six::XMLControlCreatorT<
                                       six::sicd::ComplexXMLControl>());
        xmlRegistry.addCreator(six::DataType::DERIVED,
                               new six::XMLControlCreatorT<
                                       six::sidd::DerivedXMLControl>());

        logging::Logger log;
        if (logFile == "console")
            log.addHandler(new logging::StreamHandler(logLevel), true);
        else
            log.addHandler(new logging::FileHandler(logFile, logLevel), true);

        six::NITFReadControl reader;
        reader.setLogger(&log);
        reader.setXMLControlRegistry(&xmlRegistry);

        reader.load(inputFile, schemaPaths);
        six::Container* container = reader.getContainer();

        // Update the XML to reflect the creation time as right now
        const six::DateTime now;
        for (size_t ii = 0; ii < container->getNumData(); ++ii)
        {
            container->getData(ii)->setCreationTime(now);
        }

        nitf::List imageList = reader.getRecord().getImages();
        nitf::ListIterator imageIter = imageList.begin();

        six::BufferList images;
        // Now go through every image and figure out what clump its attached
        // to and use that for the measurements
        for (size_t i = 0; imageIter != imageList.end(); ++imageIter, ++i)
        {
            // Get a segment ref
            nitf::ImageSegment segment = (nitf::ImageSegment) * imageIter;

            // Get the subheader out
            nitf::ImageSubheader subheader = segment.getSubheader();

            size_t numRows = (size_t)subheader.getNumRows();
            size_t numCols = (size_t)subheader.getNumCols();
            size_t numBPP = (size_t)
                    (((size_t)subheader.getActualBitsPerPixel() + 7) / 8);

            // read the entire image into memory
            six::Region region;
            region.setStartRow(0);
            region.setStartCol(0);
            region.setNumRows(numRows);
            region.setNumCols(numCols);

            mem::ScopedArray<sys::ubyte> imageBuffer (new sys::ubyte[
                    numRows * numCols * numBPP * 
                    (size_t)subheader.getBandCount()]);
            region.setBuffer(imageBuffer.get());
            reader.interleaved(region, 0);
            //only expand if we have a 16i image
            if (expand && numBPP == 2)
            {
                //compute the buf size and only allocate that
                size_t size = numRows * numCols;
                mem::ScopedArray<std::complex<float> > expandedBuffer (
                        new std::complex<float>[size]);

                //expand the image
                std::complex<short>* shorts = 
                        (std::complex<short>*)imageBuffer.get();
                for (size_t i = 0; i < size; i++)
                {
                    expandedBuffer.get()[i] = std::complex<float>(
                            (float)shorts[i].real(),
                            (float)shorts[i].imag());
                }

                imageBuffer.reset((sys::ubyte*)expandedBuffer.release());

                //set the new pixel type in the Data object
                container->getData(subheader.getImageId().toString(), 
                                   imageList.getSize())->setPixelType(
                                            six::PixelType::RE32F_IM32F);
            }
            images.push_back(imageBuffer.release());
        }

        six::NITFWriteControl writer;
        writer.setLogger(&log);
        writer.initialize(container);
        writer.setXMLControlRegistry(&xmlRegistry);
        writer.save(images, outputFile, schemaPaths);

        for (six::BufferList::iterator it = images.begin(); 
                it != images.end(); ++it)
            delete[] *it;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    catch (const except::Exception& ex)
    {
        std::cerr << ex.toString() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception\n";
        return 1;
    }
    return 0;
}

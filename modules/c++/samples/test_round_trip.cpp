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
#include <import/six.h>
#include <import/six/sicd.h>
#include <import/six/sidd.h>

typedef std::complex<float> ComplexFloat;

int main(int argc, char** argv)
{
    try
    {

        // create a parser and add our options to it
        cli::ArgumentParser parser;
        parser.setDescription(
                              "This program reads a SICD or SIDD into the internal "
                                  "memory model and round-trips it back to file.");
        parser.addArgument("-e --expand", "Expand RE16I_IM16I to RE32F_IM32F",
                           cli::STORE_TRUE, "expand");
        parser.addArgument("-f --log", "Specify a log file", cli::STORE, "log",
                           "FILE")->setDefault("console");
        parser.addArgument("-l --level", "Specify log level", cli::STORE,
                           "level", "LEVEL")->setChoices(
                                                         str::split(
                                                                    "debug info warn error"))->setDefault(
                                                                                                          "info");
        parser.addArgument("input", "Input SICD/SIDD", cli::STORE, "input",
                           "INPUT", 1, 1);
        parser.addArgument("output", "Output filename", cli::STORE, "output",
                           "OUTPUT", 1, 1);

        cli::Results *options = parser.parse(argc, (const char**) argv);

        std::string inputFile(options->get<std::string> ("input"));
        std::string outputFile(options->get<std::string> ("output"));
        bool expand(options->get<bool> ("expand"));
        std::string logFile(options->get<std::string> ("log"));
        std::string level(options->get<std::string> ("level"));

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

        reader.load(inputFile);
        six::Container* container = reader.getContainer();

        six::BufferList images;
        for (size_t i = 0, num = container->getNumData(); i < num; ++i)
        {
            six::Data *data = container->getData(i);

            // read the entire image into memory
            six::Region region;
            sys::ubyte *imageBuffer = reader.interleaved(region, 0);

            //only expand if we have a 16i image
            if (expand && data->getPixelType() == six::PixelType::RE16I_IM16I)
            {
                //compute the buf size and only allocate that
                unsigned long expandedSize = data->getNumRows()
                        * data->getNumCols() * sizeof(ComplexFloat);
                sys::ubyte* expandedBuffer =
                        (sys::ubyte*) sys::alignedAlloc(expandedSize);

                //expand the image
                short* shorts = (short*) &imageBuffer[0];
                ComplexFloat* cfloats = (ComplexFloat*) &expandedBuffer[0];
                for (unsigned long i = 0, sz = data->getNumRows()
                        * data->getNumCols() * 2; i < sz; i += 2)
                {
                    *cfloats++ = ComplexFloat((float) shorts[i],
                                              (float) shorts[i + 1]);
                }

                delete[] imageBuffer;
                imageBuffer = expandedBuffer;

                //set the new pixel type
                data->setPixelType(six::PixelType::RE32F_IM32F);
            }
            images.push_back(imageBuffer);
        }

        six::WriteControl *writer = new six::NITFWriteControl;
        writer->setLogger(&log);
        writer->initialize(container);
        writer->setXMLControlRegistry(&xmlRegistry);
        writer->save(images, outputFile);

        for (six::BufferList::iterator it = images.begin(); it != images.end(); ++it)
            delete[] *it;

        delete container;
        delete writer;
    }
    catch (except::Exception& ex)
    {
        std::cout << ex.toString() << std::endl;
    }
    return 0;
}

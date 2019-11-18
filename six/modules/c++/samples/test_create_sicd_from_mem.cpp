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
#include <iostream>
#include <memory>
#include <stdexcept>

#include <sys/Path.h>
#include <except/Exception.h>
#include <logging/Setup.h>
#include <cli/ArgumentParser.h>
#include <six/sicd/Utilities.h>
#include <six/sicd/ComplexXMLControl.h>
#include <six/NITFHeaderCreator.h>

#include "utils.h"

int main(int argc, char** argv)
{
    try
    {
        // create a parser and add our options to it
        cli::ArgumentParser parser;
        parser.setDescription(
                "This program creates a sample SICD NITF file of all zeros.");
        parser.addArgument("-r --rows", "Rows limit", cli::STORE, "maxRows",
                           "ROWS");
        parser.addArgument("-s --size", "Max product size", cli::STORE,
                           "maxSize", "BYTES");
        parser.addArgument("--num-rows", "Number of rows", cli::STORE,
                           "numRows", "ROWS")->setDefault(123);
        parser.addArgument("--num-cols", "Number of columns", cli::STORE,
                           "numCols", "COLS")->setDefault(456);
        parser.addArgument("--class", "Classification Level", cli::STORE,
                           "classLevel", "LEVEL")->setDefault("UNCLASSIFIED");
        parser.addArgument("--schema",
                           "Specify a schema or directory of schemas",
                           cli::STORE);
        parser.addArgument("output", "Output filename", cli::STORE, "output",
                           "OUTPUT", 1, 1);

        std::auto_ptr<cli::Results> options(parser.parse(argc, argv));

        const types::RowCol<size_t> dims(options->get<size_t>("numRows"),
                                         options->get<size_t>("numCols"));
        const std::string outputName(options->get<std::string>("output"));
        const std::string classLevel(options->get<std::string>("classLevel"));
        std::vector<std::string> schemaPaths;
        getSchemaPaths(*options, "--schema", "schema", schemaPaths);

        std::auto_ptr<logging::Logger> logger(
                logging::setupLogger(sys::Path::basename(argv[0])));

        six::XMLControlFactory::getInstance().addCreator(
                six::DataType::COMPLEX,
                new six::XMLControlCreatorT<six::sicd::ComplexXMLControl>());

        std::vector<std::complex<float> > image(dims.row * dims.col);

        std::auto_ptr<six::Data> data(
                six::sicd::Utilities::createFakeComplexData().release());
        data->setNumRows(dims.row);
        data->setNumCols(dims.col);
        data->setPixelType(six::PixelType::RE32F_IM32F);

        mem::SharedPtr<six::Container> container(new six::Container(
                six::DataType::COMPLEX));
        container->addData(data);

        /*
         *  Under normal circumstances, the library uses the
         *  segmentation algorithm in the SICD spec, and numRowsLimit
         *  is set to Contants::ILOC_SZ.  If the user sets this, they
         *  want us to create an alternate numRowsLimit to force the
         *  library to segment on smaller boundaries.
         *
         *  This is handy especially for debugging, since it will force
         *  the algorithm to segment early.
         *
         */

        six::Options writerOptions;
        if (options->hasValue("maxRows"))
        {
            std::cout << "Overriding NITF max ILOC" << std::endl;
            writerOptions.setParameter(
                    six::NITFHeaderCreator::OPT_MAX_ILOC_ROWS,
                    options->get<size_t>("maxRows"));
        }

        if (options->hasValue("maxSize"))
        {
            std::cout << "Overriding NITF product size" << std::endl;
            writerOptions.setParameter(
                    six::NITFHeaderCreator::OPT_MAX_PRODUCT_SIZE,
                    options->get<size_t>("maxSize"));
        }


        six::NITFWriteControl writer(writerOptions, container);
        writer.setLogger(logger.get());

        six::BufferList buffers;
        buffers.push_back(reinterpret_cast<six::UByte*>(&image[0]));
        writer.save(buffers, outputName, schemaPaths);

        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Caught std::exception: " << ex.what() << std::endl;
    }
    catch (const except::Exception& ex)
    {
        std::cerr << "Caught except::Exception: " << ex.getMessage()
                  << std::endl;
    }
    catch (...)
    {
        std::cerr << "Caught unknown exception\n";
    }

    return 1;
}

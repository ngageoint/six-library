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

#include <memory>
#include <iostream>
#include <stdexcept>

#include <cli/ArgumentParser.h>
#include <except/Exception.h>
#include <str/Convert.h>
#include <six/sidd/CropUtils.h>
#include <six/sidd/DerivedXMLControl.h>
#include <six/XMLControlFactory.h>
#include "utils.h"

int main(int argc, char** argv)
{
    try
    {
        // Parse the command line
        cli::ArgumentParser parser;
        parser.setDescription("This program reads a SIDD and crops it based "
                              "on a specified AOI");
        parser.addArgument("--start-row", "Start row", cli::STORE,
                           "sRow", "#");
        parser.addArgument("--start-col", "Start column", cli::STORE,
                           "sCol", "#");
        parser.addArgument("--num-rows", "Number of rows", cli::STORE,
                           "numRows", "#");
        parser.addArgument("--num-cols", "Number of columns", cli::STORE,
                           "numCols", "#");
        parser.addArgument("--schema",
                           "Specify a schema or directory of schemas",
                           cli::STORE,
                           "schema", "<directory>");
        parser.addArgument("input", "Input SIDD pathname", cli::STORE,
                           "input", "<input SIDD pathname>", 1, 1);
        parser.addArgument("output", "Output SIDD pathname", cli::STORE,
                           "output", "<output SIDD pathname>", 1, 1);

        const std::unique_ptr<cli::Results> options(parser.parse(argc, argv));
        const types::RowCol<size_t> aoiOffset(options->get<size_t>("sRow"),
                                              options->get<size_t>("sCol"));
        const types::RowCol<size_t> aoiDims(options->get<size_t>("numRows"),
                                            options->get<size_t>("numCols"));
        const std::string inPathname(options->get<std::string> ("input"));
        const std::string outPathname(options->get<std::string> ("output"));
        std::vector<std::string> schemaPaths;
        getSchemaPaths(*options, "--schema", "schema", schemaPaths);

        // Crop it
        six::XMLControlFactory::getInstance().addCreator(
                six::DataType::DERIVED,
                new six::XMLControlCreatorT<six::sidd::DerivedXMLControl>());


        six::sidd::cropSIDD(inPathname,
                            schemaPaths,
                            aoiOffset,
                            aoiDims,
                            outPathname);
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception\n";
        return 1;
    }

    return 0;
}

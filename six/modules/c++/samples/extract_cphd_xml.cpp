/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
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
#include <string.h>
#include <vector>
#include <iostream>

#define CODA_OSS_AUGMENT_std_namespace 1
#include <sys/Path.h>
#include <import/cli.h>
#include <import/io.h>

#include <cphd/CPHDReader.h>
#include <cphd/CPHDXMLControl.h>

/*!
 *  This extracts raw XML from a CPHD file using the CPHD module
 */
int main(int argc, char** argv)
{
    try
    {
        cli::ArgumentParser parser;
        parser.setDescription("Pull the XML from the CPHD imagery.");
        parser.addArgument("--no-pretty-print", "Use no formatting in byte stream",
                           cli::STORE_FALSE, "prettyPrint")->setDefault(true);
        parser.addArgument("-c --console", "Dump results to standard out",
                           cli::STORE_TRUE, "console")->setDefault(false);
        parser.addArgument("-b --basename", "Use a basename for product names",
                           cli::STORE, "basename")->setDefault("");
        parser.addArgument("cphd", "Input cphd file", cli::STORE, "cphd",
                           "", 1, 1, true);
        parser.addArgument("schema", "Input CPHD schema", cli::STORE, "schema",
                           "", 1, 1, false)->setDefault("");

        // Parse!
        const std::auto_ptr<cli::Results>
            options(parser.parse(argc, (const char**) argv));

        const bool prettyPrint = options->get<bool>("prettyPrint");
        std::string basename = options->get<std::string>("basename");
        const bool toConsole = options->get<bool>("console");
        const std::string inputFile = options->get<std::string>("cphd");
        const std::string schemaFile = options->get<std::string>("schema");

        // Check for conflicting input
        if (!basename.empty() && toConsole)
        {
            throw except::Exception(Ctxt("User cannot specify the --basename "
                                         "option while using --console"));
        }

        // Fill out basename if not user specified
        if (basename.empty())
        {
            basename = sys::Path::basename(inputFile, true);
        }
        std::string outPathname = basename  + ".xml";

        std::vector<std::string> schemaPathnames;
        if (!schemaFile.empty())
        {
            schemaPathnames.push_back(schemaFile);
        }

        // Reads in CPHD and verifies XML using schema
        cphd::CPHDReader reader(inputFile, sys::OS().getNumCPUs());
        cphd::CPHDXMLControl xmlControl;
        std::string xml = xmlControl.toXMLString(reader.getMetadata(), schemaPathnames, prettyPrint);

        std::unique_ptr<io::OutputStream> os;
        if (toConsole)
        {
            os.reset(new io::StandardOutStream());
        }
        else
        {
            os.reset(new io::FileOutputStream(outPathname));
        }
        os->write(xml);
        os->close();
        return 0;
    }
    catch (const except::Exception& e)
    {
        std::cerr << e.getMessage() << std::endl;
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception" << std::endl;
        return 1;
    }
}

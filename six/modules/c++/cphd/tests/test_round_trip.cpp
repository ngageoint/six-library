/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * cphd-c++ is free software; you can redistribute it and/or modify
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
#include <fstream>
#include <memory>
#include <logging/NullLogger.h>
#include <xml/lite/MinidomParser.h>
#include <cli/ArgumentParser.h>
#include <io/FileInputStream.h>
#include <io/FileOutputStream.h>
#include <cphd/CPHDXMLControl.h>
#include <cphd/Global.h>
#include <cphd/Metadata.h>
#include <cphd/SceneCoordinates.h>


std::string getPathname(int argc, char** argv)
{
    // Parse the command line
    if (argc != 2)
    {
        throw except::Exception(Ctxt(
                    "Invalid number of cmd line args"));
    }
    else
    {
        return argv[1];
    }
}

void parseXMLFile(xml::lite::MinidomParser& xmlParser, std::string pathname)
{
    // Parse XML file
    std::cout << "Reading file: " << pathname << "\n";
    io::FileInputStream ifs(pathname);
    xmlParser.preserveCharacterData(true);
    xmlParser.parse(ifs, ifs.available());
}

void testRoundTrip(std::string pathname, std::string outPathname, std::string schemaPathname)
{
    xml::lite::MinidomParser xmlParser;
    parseXMLFile(xmlParser, pathname);


    std::vector<std::string> schemaPaths;
    schemaPaths.push_back(schemaPathname);
    cphd::CPHDXMLControl xmlControl(new logging::NullLogger(), true, schemaPaths);

    // Populate metadata object from XML Document
    const std::auto_ptr<cphd::Metadata> metadata =
            xmlControl.fromXML(xmlParser.getDocument());

    const std::string xmlMetadata(xmlControl.toXMLString(*metadata));
    std::cout << xmlMetadata << std::endl;
    io::FileOutputStream ofs(outPathname);
    // FileHeader header;
    // // set header size, final step before write
    // header.set(xmlMetadata.size(), vbmSize, cphd03Size);
    // mFile.write(header.toString().c_str(), header.size());
    std::cout << "Writing file: " << outPathname << "\n";
    ofs.write(xmlMetadata.c_str(), xmlMetadata.size());
}

int main(int argc, char** argv)
{
    try
    {
        // Parse the command line
        cli::ArgumentParser parser;
        parser.setDescription("Round trip for a CPHD file.");
        parser.addArgument("-t --threads",
                           "Specify the number of threads to use",
                           cli::STORE,
                           "threads",
                           "NUM")->setDefault(sys::OS().getNumCPUs());
        parser.addArgument("input", "Input pathname", cli::STORE, "input",
                           "CPHD", 1, 1);
        parser.addArgument("output", "Output pathname", cli::STORE, "output",
                           "CPHD", 1, 1);
        parser.addArgument("schema", "Schema pathname", cli::STORE, "input",
                           "CPHD", 1, 1);
        const std::auto_ptr<cli::Results> options(parser.parse(argc, argv));
        const std::string inPathname(options->get<std::string>("input"));
        const std::string outPathname(options->get<std::string>("output"));
        const std::string schemaPathname(options->get<std::string>("output"));
        const size_t numThreads(options->get<size_t>("threads"));

        testRoundTrip(inPathname, outPathname, schemaPathname);
        return 0;
    }
    catch (const except::Exception& ex)
    {
        std::cerr << ex.toString() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown exception\n";
    }
    return 1;

}

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

#include <cli/ArgumentParser.h>
#include <io/TempFile.h>
#include <io/FileInputStream.h>
#include <io/FileOutputStream.h>
#include <logging/NullLogger.h>
#include <xml/lite/MinidomParser.h>

#include <cphd/CPHDXMLControl.h>
#include <cphd/Global.h>
#include <cphd/Metadata.h>
#include <cphd/SceneCoordinates.h>

#include "TestCase.h"

/*!
 * Tests write and read of metadata
 * Fails if values don't match
 */
void parseXMLFile(xml::lite::MinidomParser& xmlParser, std::string pathname)
{
    // Parse XML file
    io::FileInputStream ifs(pathname);
    xmlParser.preserveCharacterData(true);
    xmlParser.parse(ifs, ifs.available());
}

bool testEqual(const std::string& inPathname, const std::string& outPathname,
          size_t numThreads, const std::vector<std::string>& schemas)
{
    // Read in first XML file
    xml::lite::MinidomParser xmlParser;
    parseXMLFile(xmlParser, inPathname);
    cphd::CPHDXMLControl xmlControl(new logging::NullLogger(), true);
    const std::auto_ptr<cphd::Metadata> metadata =
            xmlControl.fromXML(xmlParser.getDocument(), schemas);

    const std::string xmlMetadata(xmlControl.toXMLString(*metadata));

    //Output XML file to temp file
    io::FileOutputStream ofs(outPathname);
    ofs.write(xmlMetadata.c_str(), xmlMetadata.size());

    // Read in second XML file from temp file
    xml::lite::MinidomParser xmlParser2;
    parseXMLFile(xmlParser2, outPathname);

    cphd::CPHDXMLControl xmlControl2(new logging::NullLogger(), true);

    // Populate metadata object from XML Document
    const std::auto_ptr<cphd::Metadata> metadata2 =
            xmlControl2.fromXML(xmlParser2.getDocument(), schemas);

    // Check if both metadata are equal
    return (*metadata == *metadata2);
}

void runTests(const std::string& inPathname, size_t numThreads, const std::vector<std::string>& schemas)
{
    io::TempFile tempfile;
    bool test1 = testEqual(inPathname, tempfile.pathname(), numThreads, schemas);

    if (!test1)
    {
        std::cerr << "Test Failed. Metadata are not equal. \n";
        return;
    }
    std::cout << "Metadata roundtrip passed! \n";
}

int main(int argc, char** argv)
{
    try
    {
        // Parse the command line
        cli::ArgumentParser parser;
        parser.setDescription("Round trip for an XML file.");
        parser.addArgument("-t --threads",
                           "Specify the number of threads to use",
                           cli::STORE,
                           "threads",
                           "NUM")->setDefault(sys::OS().getNumCPUs());
        parser.addArgument("input", "Input pathname", cli::STORE, "input",
                           "XML", 1, 1);
        parser.addArgument("schema", "Schema pathname", cli::STORE, "schema",
                           "XSD", 1, 1);
        const std::auto_ptr<cli::Results> options(parser.parse(argc, argv));
        const std::string inPathname(options->get<std::string>("input"));
        const std::string schemaPathname(options->get<std::string>("schema"));
        const size_t numThreads(options->get<size_t>("threads"));

        std::vector<std::string> schemas;
        if (!schemaPathname.empty())
        {
            schemas.push_back(schemaPathname);
        }
        runTests(inPathname, numThreads, schemas);
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

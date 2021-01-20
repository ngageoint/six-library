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
#include <string.h>
#include <vector>
#include <iostream>

#include <import/nitf.hpp>

#include <sys/Path.h>
#include <import/cli.h>
#include <import/xml/lite.h>
#include <import/io.h>

/*!
 *  This extracts raw XML from each NITF DES, just using nitf,
 *  xml.lite and io modules.
 */
int main(int argc, char** argv)
{
    try
    {
        cli::ArgumentParser parser;
        parser.setDescription("Pull the DES XML from the SI*D imagery.");
        parser.addArgument("--no-pretty-print", "Use no formatting in byte stream",
                           cli::STORE_FALSE, "prettyPrint")->setDefault(true);
        parser.addArgument("-c --console", "Dump results to standard out", 
                           cli::STORE_TRUE, "console")->setDefault(false);
        parser.addArgument("-b --basename", "Use a basename for product names", 
                           cli::STORE, "basename")->setDefault("");
        parser.addArgument("file", "Input SICD or SIDD file", cli::STORE, "file",
                           "", 1, 1, true);

        // Parse!
        const std::auto_ptr<cli::Results>
            options(parser.parse(argc, (const char**) argv));

        const bool prettyPrint = options->get<bool>("prettyPrint");
        std::string basename = options->get<std::string>("basename");
        const bool toConsole = options->get<bool>("console");
        const std::string inputFile = options->get<std::string> ("file");

        //! Check for conflicting input
        if (!basename.empty() && toConsole)
        {
            throw except::Exception(Ctxt("User cannot specify the --basename "
                                         "option while using --console"));
        }
        
        //! Fill out basename if not user specified
        if (basename.empty())
        {
            basename = sys::Path::basename(inputFile, true);
        }

        nitf::Reader reader;
        nitf::IOHandle io(inputFile);
        nitf::Record record = reader.read(io);

        const nitf::Uint32 numDES = record.getNumDataExtensions();
        std::vector<char> xmlVec;
        for (nitf::Uint32 ii = 0; ii < numDES; ++ii)
        {
            nitf::DESegment segment = record.getDataExtensions()[ii];
            nitf::DESubheader subheader = segment.getSubheader();

            nitf::SegmentReader deReader = reader.newDEReader(ii);
            const nitf::Off size = deReader.getSize();

            std::string typeID = subheader.getTypeID().toString();
            str::trim(typeID);

            std::string outPathname = basename + "-" + typeID + 
                                      str::toString(ii) + ".xml";

            // Read the DES
            xmlVec.resize(size);
            char* const xml = xmlVec.empty() ? NULL : &xmlVec[0];
            deReader.read(xml, size);

            // Parse it with xml::lite
            // Do this even if we're not pretty-printing to ensure the DES is
            // truly valid XML
            io::StringStream oss;
            oss.write(xml, size);

            xml::lite::MinidomParser xmlParser;
            xmlParser.parse(oss);

            std::auto_ptr<io::OutputStream> os;
            if (toConsole)
            {
                os.reset(new io::StandardOutStream());
            }
            else
            {
                os.reset(new io::FileOutputStream(outPathname));
            }

            if (prettyPrint)
            {
                // Pretty print it
                xmlParser.getDocument()->getRootElement()->prettyPrint(*os);
            }
            else
            {
                // Just dump out the raw contents
                os->write(reinterpret_cast<sys::byte*>(xml), size);
            }
            os->close();
        }
        io.close();

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

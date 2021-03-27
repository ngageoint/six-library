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
#include <stdexcept>

#include <std/filesystem>

#include <sys/Path.h>
#include <except/Exception.h>
#include <io/StandardStreams.h>
#include <six/sicd/ComplexXMLControl.h>
#include <six/sicd/Utilities.h>

namespace fs = std::filesystem;

int main(int argc, char** argv)
{
    try
    {
        // Parse the command line
        const std::string progname(argv[0]);
        if (argc != 2 && argc != 3)
        {
            std::cerr << "Usage: " << fs::path(progname).filename().string()
                      << " <SICD pathname> [<schema dirname>]\n\n";
            return 1;
        }

        const std::string sicdPathname(argv[1]);

        std::vector<std::string> schemaPaths;
        if (argc == 3)
        {
            schemaPaths.push_back(argv[2]);
        }
        else
        {
            // In a normal installation, we can infer the path
            const auto progDirname = fs::path(progname).parent_path();
            const fs::path schemaPath = progDirname.parent_path().parent_path() / "conf" / "schema" / "six";
            schemaPaths.push_back(fs::absolute(schemaPath).string());
        }

        // Parse out the XML into a C++ structure
        std::unique_ptr<six::sicd::ComplexData> data =
                six::sicd::Utilities::getComplexData(sicdPathname,
                                                     schemaPaths);

        // Normally at this point you'd use the C++ structure, but at this
        // point let's convert it back to XML and print it out just to show
        // we really parsed it
        six::sicd::ComplexXMLControl xmlControl;
        std::unique_ptr<xml::lite::Document>
                xmlDoc(xmlControl.toXML(data.get(), schemaPaths));

        io::StandardOutStream oss;
        xmlDoc->getRootElement()->prettyPrint(oss);

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

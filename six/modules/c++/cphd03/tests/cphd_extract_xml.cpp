/* =========================================================================
 * This file is part of cphd03-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * cphd03-c++ is free software; you can redistribute it and/or modify
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

#include <six/sys_Conf.h>
#include <sys/Path.h>
#include <except/Exception.h>
#include <mem/ScopedArray.h>
#include <io/FileInputStream.h>
#include <xml/lite/MinidomParser.h>
#include <cphd03/FileHeader.h>

#include <sys/Filesystem.h>
namespace fs = std::filesystem;

int main(int argc, char** argv)
{
    try
    {
        // Parse the command line
        const auto progname(fs::path(argv[0]).filename());
        if (argc != 2)
        {
            std::cerr << "Usage: " << progname << " <CPHD pathname>\n\n";
            return 1;
        }
        const std::string pathname(argv[1]);

        // Read the file header to determine where the XML section is
        io::FileInputStream inStream(pathname);
        cphd03::FileHeader fileHeader;
        fileHeader.read(inStream);

        // Read in the XML section, parsing it through xml.lite as we go
        inStream.seek(fileHeader.getXMLoffset(), io::Seekable::START);
        xml::lite::MinidomParser parser;
        parser.parse(inStream, fileHeader.getXMLsize());

        // Pretty print it out to stdout
        io::StandardOutStream outStream;
        parser.getDocument()->getRootElement()->prettyPrint(outStream);

        return 0;
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
}

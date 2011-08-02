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
#include <string.h>
#include <vector>
#include <iostream>

#include <import/nitf.hpp>
#include <import/xml/lite.h>
#include <import/io.h>

/*!
 *  This extracts raw XML from each NITF DES, just using nitf,
 *  xml.lite and io modules.
 */

namespace
{
static const char* USAGE =
    "Usage: %s [--no-pretty-print] <siXd-image-file>\n";
}

int main(int argc, char** argv)
{

    if (argc != 2 && argc != 3)
        die_printf(USAGE, argv[0]);

    try
    {
        bool prettyPrint;
        int idx(1);
        if (argc == 2)
        {
            prettyPrint = true;
        }
        else
        {
            if (::strcmp(argv[idx++], "--no-pretty-print") != 0)
            {
                die_printf(USAGE, argv[0]);
            }
            prettyPrint = false;
        }
        const std::string inputFile = argv[idx++];
        const std::string prefix = sys::Path::basename(inputFile, true);

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
            const std::string outputFile =
                FmtX("%s-%s%d.xml", prefix.c_str(), typeID.c_str(), ii);

            // Read the DES
            xmlVec.resize(size);
            char * const xml = xmlVec.empty() ? NULL : &xmlVec[0];
            deReader.read(xml, size);

            // Parse it with xml::lite
            // Do this even if we're not pretty-printing to ensure the DES is
            // truly valid XML
            io::ByteStream bs;
            bs.write(xml, size);

            xml::lite::MinidomParser parser;
            parser.parse(bs);

            io::FileOutputStream fos(outputFile);
            if (prettyPrint)
            {
                // Pretty print it
                parser.getDocument()->getRootElement()->prettyPrint(fos);
            }
            else
            {
                // Just dump out the raw contents
                fos.write(reinterpret_cast<sys::byte*>(xml), size);
            }
            fos.close();
        }
        io.close();
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

    return 0;
}

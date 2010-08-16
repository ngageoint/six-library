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
#include <import/nitf.hpp>
#include <import/xml/lite.h>
#include <import/io.h>

/*!
 *  This extracts raw XML from each NITF DES, just using nitf,
 *  xml.lite and io modules.
 */

const char* USAGE =
        "Usage: %s <siXd-image-file>\n";

int main(int argc, char** argv)
{

    if (argc != 2)
        die_printf(USAGE, argv[0]);

    std::string inputFile = argv[1];
    std::string prefix = sys::Path::basename(inputFile, true);
    try
    {
        nitf::Reader reader;
        nitf::IOHandle io(inputFile);
        nitf::Record record = reader.read(io);

        nitf::Uint32 numDES =  record.getNumDataExtensions();
        for (nitf::Uint32 i = 0; i < numDES; ++i)
        {
            nitf::DESegment segment = record.getDataExtensions()[i];
            nitf::DESubheader subheader = segment.getSubheader();

            nitf::SegmentReader deReader = reader.newDEReader(i);
            nitf::Off size = deReader.getSize();

            std::string typeID = subheader.getTypeID().toString();
            str::trim(typeID);
            std::string fileName = FmtX("%s-%s%d.xml", prefix.c_str(),
                                        typeID.c_str(), i);

            char* xml = new char[size];
            deReader.read(xml, size);

            io::ByteStream bs;
            bs.write(xml, size);

            xml::lite::MinidomParser parser;
            parser.parse(bs);

            xml::lite::Document* doc = parser.getDocument();


            io::FileOutputStream fos(fileName);
            doc->getRootElement()->prettyPrint(fos);
            fos.close();
            delete [] xml;
        }
        io.close();
    }
    catch (except::Exception& e)
    {
        std::cout << e.getMessage() << std::endl;
    }
}

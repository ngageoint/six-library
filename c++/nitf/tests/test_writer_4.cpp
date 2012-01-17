/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include <import/nitf.hpp>
#include <iostream>
#include <string>

int main(int argc, char **argv)
{
    try
    {
        if (argc != 3)
        {
            std::cout << "Usage: " << argv[0]
                      << " <input-file> <output-file>\n\n";
            exit(EXIT_FAILURE);
        }

        //create a Reader and read in the Record
        nitf::Reader reader;
        nitf::IOHandle input(argv[1]);
        nitf::Record record = reader.read(input);

        //create a Writer and prepare it for the Record
        nitf::IOHandle output(argv[2], NITF_ACCESS_WRITEONLY, NITF_CREATE);
        nitf::Writer writer;
        writer.prepare(output, record);

        //go through and setup the pass-through writehandlers
        nitf::List images = record.getImages();
        for (int i = 0, num = record.getHeader().getNumImages(); i < num; ++i)
        {
            nitf::ImageSegment segment = images[i];
            long offset = segment.getImageOffset();
            nitf::StreamIOWriteHandler handler(input, offset,
                            segment.getImageEnd() - offset);
            writer.setImageWriteHandler(i, handler);
        }

        nitf::List graphics = record.getImages();
        for (int i = 0, num = record.getHeader().getNumGraphics(); i < num; ++i)
        {
            nitf::GraphicSegment segment = graphics[i];
            long offset = segment.getOffset();
            nitf::StreamIOWriteHandler handler(input, offset,
                            segment.getEnd() - offset);
            writer.setGraphicWriteHandler(i, handler);
        }

        nitf::List texts = record.getTexts();
        for (int i = 0, num = record.getHeader().getNumTexts(); i < num; ++i)
        {
            nitf::TextSegment segment = texts[i];
            long offset = segment.getOffset();
            nitf::StreamIOWriteHandler handler(input, offset,
                            segment.getEnd() - offset);
            writer.setTextWriteHandler(i, handler);
        }

        //once everything is set, write it!
        writer.write();
        input.close();
        output.close();
    }
    catch (except::Throwable & t)
    {
        std::cout << t.getMessage() << std::endl;
    }
}

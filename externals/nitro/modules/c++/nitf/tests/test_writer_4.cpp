/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
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

#include <mem/SharedPtr.h>
#include <sys/Path.h>
#include <import/nitf.hpp>
#include <iostream>
#include <string>

#include <sys/Filesystem.h>
namespace fs = std::filesystem;

int main(int argc, char **argv)
{
    try
    {
        if (argc != 3)
        {
            std::cerr << "Usage: " << fs::path(argv[0]).filename().string()
                      << " <input-file> <output-file>\n\n";
            return 1;
        }

        //create a Reader and read in the Record
        nitf::Reader reader;
        nitf::IOHandle input(argv[1]);
        nitf::Record record = reader.read(input);

        //create a Writer and prepare it for the Record
        nitf::IOHandle output(argv[2], NITF_ACCESS_WRITEONLY, NITF_CREATE);
        nitf::Writer writer;
        writer.prepare(output, record);
        writer.setWriteHandlers(input, record);

        //once everything is set, write it!
        writer.write();
        return 0;
    }
    catch (const except::Throwable& th)
    {
        std::cerr << th.getMessage() << std::endl;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "An unknown exception occured\n";
    }
    return 1;
}

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

void testRecord(nitf::Reader reader)
{
    std::cout << reader.getRecord().getHeader().getFileTitle().toString() << std::endl;
}

int main(int argc, char **argv)
{
    try
    {
        //  This is the reader object
        nitf::Reader reader;

        if (argc != 2)
        {
            std::cout << "Usage: " << argv[0] << " <nitf-file>" << std::endl;
            exit( EXIT_FAILURE);
        }

        if (nitf::Reader::getNITFVersion(argv[1]) == NITF_VER_UNKNOWN)
        {
            std::cout << "This file does not appear to be a valid NITF"
                    << std::endl;
            exit( EXIT_FAILURE);
        }

        nitf::IOHandle io(argv[1]);
        reader.read(io);

        testRecord(reader);
        io.close();

        for (int i = 0; i < 2000; ++i)
        {
            nitf::IOHandle handle(argv[1]);
//            handle.close();
        }


        return 0;
    }
    catch (except::Throwable& t)
    {
        std::cout << t.getTrace() << std::endl;
        return 1;
    }
}

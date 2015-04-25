/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

// Demonstrates how to add a TRE to the file header's user-defined section
// Adding TREs to other subheaders, such as an image subheader, is very
// similar to this

#include <iostream>
#include <string>

#include <sys/Path.h>
#include <except/Exception.h>
#include <nitf/Writer.hpp>

int main(int argc, char** argv)
{
    try
    {
        // Parse the command line
        if (argc != 2)
        {
            std::cerr << "Usage: " << sys::Path::basename(argv[0])
                      << " <output pathname>\n\n";
            return 1;
        }
        const std::string outPathname(argv[1]);

        // Add a TRE to the file header's user-defined section
        // Any fields we don't fill in will be filled with 0's for BCS-N or
        // spaces for BCS-A
        nitf::Record record;
        nitf::TRE tre("PIAIMB", "PIAIMB");
        tre.setField("cloud", "ABC");
        tre.setField("stdrd", "x");
        tre.setField("smode", "Some Mode");
        tre.setField("sname", "Some Name");
        tre.setField("srce", "This is my source");
        record.getHeader().getUserDefinedSection().appendTRE(tre);

        // Write out the file
        nitf::Writer writer;
        nitf::IOHandle outIO(outPathname, NITF_ACCESS_WRITEONLY, NITF_CREATE);
        writer.prepare(outIO, record);
        writer.write();

        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Caught std::exception: " << ex.what() << std::endl;
        return 1;
    }
    catch (const except::Exception& ex)
    {
        std::cerr << "Caught except::exception: " << ex.getMessage()
                  << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Caught unknown exception\n";
        return 1;
    }

}
